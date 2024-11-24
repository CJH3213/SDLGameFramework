#include "AudioManager.h"

extern "C"
{
	#include <SDL_assert.h>
	#include <SDL_log.h>
	#include <SDL.h>
}

#include <algorithm>


AudioManager::AudioManager()
{
	SDL_zero(mDesiredSpec);
}

AudioManager& AudioManager::GetInstance()
{
    static AudioManager instance;
    return instance;
}

void AudioManager::AudioCallback(void* userdata, uint8_t* stream, int len)
{
	//回调后先手动清零音频流
	SDL_memset(stream, 0, len);

	auto ins = (AudioManager*)userdata;
	auto& packages = ins->mAudioPackages;

	std::lock_guard<std::mutex> lock(ins->mMutex);

	for (auto it = packages.begin(); it != packages.end();)
	{
		auto& pack_wp = (*it);
		// 剔除失效音频包裹
		if (pack_wp.expired() == true)
		{
			it = packages.erase(it);
			continue;
		}

		auto pack = pack_wp.lock();
		// 该音频包是否运许播放
		if (pack->mIsPlaying == true)
		{
			size_t dataLen = pack->mData.size();
			size_t currIndex = pack->mPlaybackProgress;

			// 如果当前音频数据已经播放完毕，是否循环播放
			if (currIndex >= dataLen)
				if (pack->mIsLoop == false)
				{
					pack->mIsPlaying = false;
					it = packages.erase(it);	// 播放完毕且不循环，剔除
					continue;
				}
				else
					pack->mPlaybackProgress = 0;

			// 剩余字节是否小于缓冲区大小
			size_t fillLen = std::min<size_t>(len, dataLen - currIndex);
			pack->mPlaybackProgress += fillLen;
			// 将新音频数据装载到音频流缓存区
			SDL_MixAudio(stream, pack->mData.data() + currIndex, fillLen,
				SDL_MIX_MAXVOLUME * pack->mVolume);
		}

		it++;
	}
}

void AudioManager::Init()
{
	auto& ins = GetInstance();

	//if (ins.mIsOpened == true)
	//{
	//	SDL_CloseAudio();
	//}

	// 填入期望的播放参数
	ins.mDesiredSpec.freq = 44100;	// 采样数 每秒
	ins.mDesiredSpec.format = AUDIO_S16;	//采样格式：默认PCM有符格式 | 位宽
	ins.mDesiredSpec.channels = 2;		// 声道
	ins.mDesiredSpec.silence = 0;	// 静音值（对于无音频内容的时候的取值）
	ins.mDesiredSpec.samples = 1024;	// 每通道缓存区开辟大小，大了占空间，小了频繁回调
	// size = samples * channels * (bits per sample / 8)
	ins.mDesiredSpec.callback = AudioCallback;
	ins.mDesiredSpec.userdata = &ins;		//将当前对象作为用户数据传入

	// 如果给第二个参数赋值，就需要手动转码，否则没有声音 
	//SDL_PauseAudio(1);
	int res = SDL_OpenAudio(&(ins.mDesiredSpec), NULL);
	SDL_PauseAudio(0);
	
	// ins.mIsOpened = true;
}

void AudioManager::Close()
{
	auto& ins = GetInstance();

	std::lock_guard<std::mutex> lock(ins.mMutex);
	ins.mAudioPackages.clear();
	SDL_CloseAudio();	// 会出现警告，未知
}

void AudioManager::Play(std::weak_ptr<AudioPackage> pack_wp)
{
	auto& ins = GetInstance();
	auto& packs = ins.mAudioPackages;
	auto& desiredSpec = ins.mDesiredSpec;
	auto pack = pack_wp.lock();

	if (!pack) return;

	//auto it = std::find(packs.begin(), packs.end(), pack);
	//// 已经存在，不要重复添加
	//if (it != packs.end())
	//	return;

	// 未存在，添加音频到容器
	// 比较音频和播放器格式，不相同就需要转换
	if (pack->mDesiredSpec.freq != desiredSpec.freq ||
		pack->mDesiredSpec.format != desiredSpec.format ||
		pack->mDesiredSpec.channels != desiredSpec.channels)
	{
		SDL_AudioCVT cvt;
		int res = SDL_BuildAudioCVT(&cvt,
			pack->mDesiredSpec.format, pack->mDesiredSpec.channels, pack->mDesiredSpec.freq,
			desiredSpec.format, desiredSpec.channels, desiredSpec.freq);

		SDL_assert(cvt.needed);
		cvt.len = pack->mData.size();		// 源数据大小
		pack->mData.resize(cvt.len * cvt.len_mult);	// 预备缓存区大小 = 原大小 * 扩大比例
		cvt.buf = pack->mData.data();	// 传入缓存区，请先将原数据复制到缓存区，转换后的数据将会覆盖
		cvt.len_cvt;

		int res2 = SDL_ConvertAudio(&cvt);

		// 获取转换完成后大小不要再用cvt.len * cvt.len_ratio，cvt.len_ratio是随机值。直接使用cvt.len_cvt就行
		pack->mData.resize(cvt.len_cvt);	// 转换后的有效字节数 = 原大小 * 缩放比率
		pack->mDesiredSpec = desiredSpec;
	}

	// 添加到音频包容器
	std::lock_guard<std::mutex> lock(ins.mMutex);
	packs.emplace_back(pack);

	pack->mIsPlaying = true;
}

void AudioManager::Remove(AudioPackage pack)
{
	auto& ins = GetInstance();
	auto& packs = ins.mAudioPackages;
	auto pAP = &pack;

	std::lock_guard<std::mutex> lock(ins.mMutex);

	auto it = std::find_if(packs.begin(), packs.end(),
		[&pAP](std::weak_ptr<AudioPackage>& ren) {return pAP == ren.lock().get(); });

	if (it != packs.end())
		packs.erase(it);
}

std::shared_ptr<AudioPackage> AudioManager::LoadWav(std::string path)
{
	// 加载WAV音频文件信息和数据
	SDL_AudioSpec audioSpec;
	uint8_t* buf = nullptr;
	uint32_t len = 0;
	auto res = SDL_LoadWAV(path.c_str(), &audioSpec, &buf, &len);

	// 将SDL音频数据打包为AudioPackage
	if (res == NULL)
	{
		SDL_Log("WAV音频加载失败：%s", path.c_str());
		return nullptr;
	}

	auto pack = std::make_shared<AudioPackage>();
	pack->mDesiredSpec = audioSpec;
	pack->mData.resize(len);
	memcpy(pack->mData.data(), buf, len);

	// 释放WAV加载的资源
	SDL_FreeWAV(buf);
	return pack;
}





extern "C"
{
	#include <libavutil/frame.h>
	#include <libavutil/mem.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/imgutils.h>
}
#include "WindowsManager.h"

/*以下封装FFMpeg的一些对象，方便RAII自动释放内存*/ 

struct AVFormatContext_ptr
{
	AVFormatContext* mAVFormatContext;
	AVFormatContext_ptr() { mAVFormatContext = avformat_alloc_context(); }
	~AVFormatContext_ptr() { avformat_close_input(&mAVFormatContext); avformat_free_context(mAVFormatContext); }
	AVFormatContext* operator->() { return mAVFormatContext; }
	int OpenInput(const char* url, AVInputFormat* fmt, AVDictionary** options)
	{
		return avformat_open_input(&mAVFormatContext, url, fmt, options);
	}
	int FindStreamInfo(AVDictionary** options)
	{
		return avformat_find_stream_info(mAVFormatContext, options);
	}
	void DumpFormat(int index, const char* url, int is_output)
	{
		av_dump_format(mAVFormatContext, index, url, is_output);
	}
	int ReadFrame(AVPacket* pkt)
	{
		return av_read_frame(mAVFormatContext, pkt);
	}
};

struct AVPacket_ptr
{
	AVPacket* mAVPacket;
	AVPacket_ptr() { mAVPacket = av_packet_alloc(); }
	~AVPacket_ptr() { av_packet_free(&mAVPacket); }
	AVPacket* operator->() { return mAVPacket; }
	AVPacket* operator*() { return mAVPacket; }
	void Unref() { av_packet_unref(mAVPacket); }
};

struct AVCodecContext_ptr
{
	AVCodecContext* mAVCodecContext;
	AVCodecContext_ptr(const AVCodec* codec){ mAVCodecContext = avcodec_alloc_context3(codec); }
	~AVCodecContext_ptr() { avcodec_free_context(&mAVCodecContext); }
	AVCodecContext* operator->() { return mAVCodecContext; }
	AVCodecContext* operator*() { return mAVCodecContext; }
	int ParametersToContext(const AVCodecParameters* par)
	{
		return avcodec_parameters_to_context(mAVCodecContext, par);
	}
	int Open2(const AVCodec* codec, AVDictionary** options)
	{
		return avcodec_open2(mAVCodecContext, codec, options);
	}
};

struct AVFrame_ptr
{
	AVFrame* mAVFrame;
	AVFrame_ptr() { mAVFrame = av_frame_alloc(); }
	~AVFrame_ptr(){ av_frame_free(&mAVFrame); }
	AVFrame* operator->() { return mAVFrame; }
	AVFrame* operator*() { return mAVFrame; }
};


// 将 FFMpeg的音频格式码 转为 SDL音频格式码
SDL_AudioFormat AVSampleFormatToSDLAudioFormat(AVSampleFormat ffmpegAudioFormat)
{
	switch (ffmpegAudioFormat)
	{
	case AV_SAMPLE_FMT_U8:	
	case AV_SAMPLE_FMT_U8P:		return AUDIO_U8;
	case AV_SAMPLE_FMT_S16:
	case AV_SAMPLE_FMT_S16P:	return AUDIO_S16;
	case AV_SAMPLE_FMT_S32:
	case AV_SAMPLE_FMT_S32P:	return AUDIO_S32;
	case AV_SAMPLE_FMT_FLT:
	case AV_SAMPLE_FMT_FLTP:	return AUDIO_F32;
	default:
		return 0;
	}
}

// 音频解码部分
void AudioDecode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, std::vector<uint8_t>& outData)
{
	if (!dec_ctx || !pkt || !frame)
		return;

	int ret;

	// 外部将原数据解析好之后，存入数据包

	// 将数据包发给解码器，MP3音频数据->解码器->PCM音频数据
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) 
	{
		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return;
	}

	// 从编码器接收所有数据帧，接收解码后的数据帧
	while (ret >= 0) 
	{
		ret = avcodec_receive_frame(dec_ctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) 
		{
			fprintf(stderr, "Error during decoding\n");
			return;
		}

		// 从解码器中获取采样格式，多少字节 每次 采样（无符号有符号，8位16位，整型数浮点数）
		 int data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
		if (data_size < 0) 
		{
			fprintf(stderr, "Failed to calculate data size\n");
			return;
		}
		
		// 计算帧里实际有几个通道的数据
		int frameChannels = 0;
		for (; frameChannels < AV_NUM_DATA_POINTERS; frameChannels++)
			if (frame->data[frameChannels] == nullptr) break;

		// 扩容接收容器
		auto iOutData = outData.size();	// 上次已使用的字节数
		auto iThisBytes = frame->nb_samples * data_size * dec_ctx->channels;		// 本次新增字节数
		outData.resize(outData.size() + iThisBytes);	// resize扩容后size也会增加，而reserve只会扩容不影响size
		auto pOutData = outData.data() + iOutData;	// 获取容器内部指针，并移动到本次插入初始位置
		
		// 将解码后的数据写入文件
		if (frameChannels == 1)	// 单声道 或 非平行存储时，解码后数据只存在data[0]，直接搬运
			memcpy(pOutData, frame->data[0], iThisBytes);
		else
			for (int si = 0; si < frame->nb_samples; si++)
				for (int ch = 0; ch < dec_ctx->channels; ch++)
					for (int bi = 0; bi < data_size; bi++)
						*(pOutData++) = frame->data[ch][data_size * si + bi];	//这里如果用emplace_back效率慢到吓人，直接写入内部数组非常快
	}
}

// 图片解码部分
void ImageDecode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, Texture& outData)
{
	if (!dec_ctx || !pkt || !frame || !outData)
		return;

	int ret;

	// 外部将原数据解析好之后，存入数据包

	// 将数据包发给解码器，MP3音频数据->解码器->PCM音频数据
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0)
	{
		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return;
	}

	// 从编码器接收所有数据帧，接收解码后的数据帧
	while (ret >= 0)
	{
		ret = avcodec_receive_frame(dec_ctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0)
		{
			fprintf(stderr, "Error during decoding\n");
			return;
		}

		// 从解码器中获取采样格式，多少字节 每次 采样（无符号有符号，8位16位，整型数浮点数）
		int data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
		if (data_size < 0)
		{
			fprintf(stderr, "Failed to calculate data size\n");
			return;
		}

		// 计算帧里实际有几个通道的数据
		int frameChannels = 0;
		for (; frameChannels < AV_NUM_DATA_POINTERS; frameChannels++)
			if (frame->data[frameChannels] == nullptr) break;

		SwsContext* swsCtx = sws_getContext(
			dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt, 
			dec_ctx->width, dec_ctx->height, AV_PIX_FMT_ABGR, SWS_BICUBIC, NULL, NULL, NULL);
		
		// 注意：SDL_RGBA = FFMpeg_ABGR，相反的，是因为二者的格式名称具有不同含义：
		// SDL_RGBA是表示字节高低关系，R在高字节，A在低字节，
		// FFMpeg_ABGR是表示内存地址高低关系，A|B|G|R|A|B|G|R...，A在低地址，R在高地址
		// 在Windows、Linux系统等小端存储系统，低字节在低地址，高字节在高地址，SDL_RGBA = FFMpeg_ABGR
		// 在大端存储系统，低字节在高地址，高字节在低地址，SDL_RGBA = FFMpeg_RGBA
		// 元素储存还分为交叉和平行两种，交叉是RGB|RGB|RGB，平行是RRR...GGG...BBB...
		// 在平行时不需要注意大小端问题，所以SDL_RGB24 = FFMpeg_RGB24
		// SDL还有一个坑：SDL_PIXELFORMAT_BGR888 = SDL_PIXELFORMAT_XBGR8888 其实是占4个字节

		uint8_t* pointers[4];
		int linesizes[4];
		av_image_alloc(pointers, linesizes, dec_ctx->width, dec_ctx->height, AV_PIX_FMT_ABGR, 1);
		sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, pointers, linesizes);

		// 将解码后的数据写入文件
		uint32_t f;
		int w, h;
		SDL_QueryTexture(outData.GetSDLTexture(), &f, NULL, &w, &h);
		SDL_UpdateTexture(outData.GetSDLTexture(), NULL, pointers[0], linesizes[0]);
		/*SDL_UpdateYUVTexture(outData.GetSDLTexture(), NULL,
			frame->data[0], frame->linesize[0],
			frame->data[1], frame->linesize[1],
			frame->data[2], frame->linesize[2]);*/

		av_freep(&pointers[0]);
		sws_freeContext(swsCtx);
	}
}

// 传入音频文件路径，返回解码后的音频数据包
std::shared_ptr<AudioPackage> DecodeAudioFile(const char* intputFile)
{
	Texture coverImage;

	// FFmpeg对UTF8支持最好，但是GBK路径传入也能读到文件，就是路径的中文会乱码
	// 但是读取文件内的中文不会乱码

	// 从文件中读出格式
	AVFormatContext_ptr formatContext;
	if (formatContext.OpenInput(intputFile, NULL, NULL) != 0)	// 打开文件
	{
		fprintf(stderr, "Could not open file\n");
		return nullptr;
	}
	if (formatContext.FindStreamInfo(NULL) != 0)		// 获取文件中有哪些流
	{
		fprintf(stderr, "Could not find stream info\n");
		return nullptr;
	}
	formatContext.DumpFormat(0, intputFile, 0);		// 输出信息

	// 寻找其中的音频流，第一个找到的音频流
	int audioStreamIndex = -1;
	AVCodecParameters* pParameters = nullptr;
	for (int i = 0; i < formatContext->nb_streams; i++)
	{
		auto stream = formatContext->streams[i];
		if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStreamIndex = i;
			pParameters = stream->codecpar;
			//break;
		}
		else if (stream->disposition & AV_DISPOSITION_ATTACHED_PIC)
		{
			AVPacket& pkt = stream->attached_pic;
			AVCodecParameters* codecPar = stream->codecpar;
			const AVCodec* codec = avcodec_find_decoder(codecPar->codec_id);
			AVCodecContext_ptr codecContext(codec);
			codecContext.ParametersToContext(codecPar);
			codecContext.Open2(codec, NULL);
			AVFrame_ptr decodedFrame;
			//AVPacket_ptr packet;
			coverImage = WindowsManager::Get(0).CreateTextureForImage(Vector2(codecContext->width, codecContext->height));
			ImageDecode(*codecContext, &pkt, *decodedFrame, coverImage);
		}
	}
	if (pParameters == nullptr)
	{
		fprintf(stderr, "AUDIO stream not found\n");
		return nullptr;
	}

	// 查找mp3解码器
	const AVCodec* codec = avcodec_find_decoder(pParameters->codec_id);
	if (codec == nullptr) 
	{
		fprintf(stderr, "Codec not found\n");
		return nullptr;
	}

	// 分配，解码器 上下文
	AVCodecContext_ptr codecContext(codec);
	if (*codecContext == nullptr) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		return nullptr;
	}

	// 将 文件的参数 复制到 解码器上下文
	codecContext.ParametersToContext(pParameters);

	// 打开解码器
	if (codecContext.Open2(codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return nullptr;
	}
	
	//	// 只在第一次循环，分配解码器 的 帧
	AVFrame_ptr decodedFrame;

	// 分配数据包
	AVPacket_ptr packet;

	// 构造游戏的音频包裹，填入音频参数
	auto audioPck = std::make_shared<AudioPackage>();
	audioPck->mDesiredSpec.freq = pParameters->sample_rate;
	audioPck->mDesiredSpec.format = AVSampleFormatToSDLAudioFormat((AVSampleFormat)pParameters->format);
	audioPck->mDesiredSpec.channels = pParameters->channels;
	audioPck->mCoverImage = coverImage;

	while (formatContext.ReadFrame(*packet) >= 0)
	{
		if (packet->stream_index == audioStreamIndex)
			AudioDecode(*codecContext, *packet, *decodedFrame, audioPck->mData);
		packet.Unref();
	}

	return audioPck;
}

std::shared_ptr<AudioPackage> AudioManager::LoadAudioFile(std::string path)
{
	auto pack = DecodeAudioFile(path.c_str());

	return pack;
}

double AudioPackage::GetDuration() const
{
	return (double)mData.size() /
		(SDL_AUDIO_BITSIZE(mDesiredSpec.format) / 8 * mDesiredSpec.channels * mDesiredSpec.freq);
}
