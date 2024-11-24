#include "AudioManager.h"
#include <SDL_assert.h>
#include <SDL_log.h>
#include <SDL.h>

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
		size_t dataLen = pack->mData.size();
		size_t currIndex = pack->mPlaybackProgress;

		// 如果当前音频数据已经播放完毕，是否循环播放
		if (currIndex >= dataLen)
			if (pack->mIsLoop == false)
			{
				it = packages.erase(it);	// 播放完毕且不循环，剔除
				continue;
			}
			else
				pack->mPlaybackProgress = 0;

		// 剩余字节是否小于缓冲区大小
		size_t remainingLen = dataLen - currIndex;
		size_t fillLen = len < remainingLen ? len : remainingLen;

		// 将新音频数据装载到音频流缓存区
		SDL_MixAudio(stream, pack->mData.data() + currIndex, fillLen,
			SDL_MIX_MAXVOLUME * pack->mVolume);

		// 音频指针增长
		currIndex += len;
		pack->mPlaybackProgress += fillLen;

		// 显示当前音频的进度
		//SDL_Log(u8"播放进度：%d", pack->mPlaybackProgress);

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
	std::memcpy(pack->mData.data(), buf, len);

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
}

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

SDL_AudioFormat AVSampleFormatToSDLAudioFormat(AVSampleFormat ffmpegAudioFormat)
{
	switch (ffmpegAudioFormat)
	{
	case AV_SAMPLE_FMT_U8:	
	case AV_SAMPLE_FMT_U8P:		return AUDIO_U8;
	case AV_SAMPLE_FMT_S16:
	case AV_SAMPLE_FMT_S16P:		return AUDIO_S16;
	case AV_SAMPLE_FMT_S32:
	case AV_SAMPLE_FMT_S32P:		return AUDIO_S32;
	case AV_SAMPLE_FMT_FLT:
	case AV_SAMPLE_FMT_FLTP:		return AUDIO_F32;
	default:
		return 0;
	}
}

void decode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, std::vector<uint8_t>& outData)
{
	int ret;

	// 外部将原数据解析好之后，存入数据包

	// 将数据包发给解码器，MP3音频数据->解码器->PCM音频数据
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return;
	}

	// 从编码器接收所有数据帧，接收解码后的数据帧
	while (ret >= 0) {
		ret = avcodec_receive_frame(dec_ctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}

		// 从解码器中获取采样格式，多少字节 每次 采样（无符号有符号，8位16位，整型数浮点数）
		 int data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
		if (data_size < 0) {
			/* 不应该出现的情况 */
			fprintf(stderr, "Failed to calculate data size\n");
			exit(1);
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

std::shared_ptr<AudioPackage> DecodeAudioFile(const char* intputFile)
{
	// FFmpeg对UTF8支持最好，但是GBK路径传入也能读到文件，就是路径的中文会乱码
	// 但是读取文件内的中文不会乱码

	FILE* f;
	uint8_t* inbuf = new uint8_t[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
	AVFrame* decoded_frame = NULL;

	//av_register_all();
	// 从文件中读出格式
	AVFormatContext* fmt_ctx = avformat_alloc_context();
	int res = avformat_open_input(&fmt_ctx, intputFile, NULL, NULL);
	res = avformat_find_stream_info(fmt_ctx, NULL);		// 获取文件中有哪些流
	av_dump_format(fmt_ctx, 0, intputFile, 0);		// 输出信息

	// 寻找其中的音频流，第一个找到的音频流
	int audioStreamIndex = -1;
	AVCodecParameters* pParameters = nullptr;
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStreamIndex = i;
			pParameters = fmt_ctx->streams[i]->codecpar;
			break;
		}
	if (pParameters == nullptr)
		exit(1);

	// 构造游戏的音频包裹，填入音频参数
	auto audioPck = std::make_shared<AudioPackage>();
	//SDL_memset(&audioPck->mDesiredSpec, 0, sizeof(SDL_AudioSpec));
	audioPck->mDesiredSpec.freq = pParameters->sample_rate;
	audioPck->mDesiredSpec.format = AVSampleFormatToSDLAudioFormat((AVSampleFormat)pParameters->format);
	audioPck->mDesiredSpec.channels = pParameters->channels;

	// 分配数据包
	AVPacket* pkt = av_packet_alloc();

	// 查找mp3解码器
	const AVCodec* codec = avcodec_find_decoder(pParameters->codec_id);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}

	// 根据解码器ID（文件格式），初始化裸流解析器，
	//AVCodecParserContext* parser = av_parser_init(codec->id);
	//if (!parser) {
	//	fprintf(stderr, "Parser not found\n");
	//	exit(1);
	//}

	// 分配，解码器 上下文
	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		exit(1);
	}

	// 将 文件的参数 复制到 解码器上下文
	avcodec_parameters_to_context(c, pParameters);
	c->channel_layout = AV_CH_LAYOUT_STEREO;

	// 打开解码器
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	
	//	// 只在第一次循环，分配解码器 的 帧
	if (!(decoded_frame = av_frame_alloc())) {
		fprintf(stderr, "Could not allocate audio frame\n");
		exit(1);
	}

	// 打开输入输出文件
	//fopen_s(&f, intputFile, "rb");		// 如中文路径需GBK编码
	//if (!f) {
	//	fprintf(stderr, "Could not open %s\n", intputFile);
	//	exit(1);
	//}

	// 首次读取原音频数据
	//uint8_t* data = inbuf;
	//size_t data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);

	//while (data_size > 0) {

	//	// 只在第一次循环，分配解码器 的 帧
	//	if (!decoded_frame) {
	//		if (!(decoded_frame = av_frame_alloc())) {
	//			fprintf(stderr, "Could not allocate audio frame\n");
	//			exit(1);
	//		}
	//	}

	//	// 原文件数据->解析器->MP3编码数据，解析一次原始数据 到数据包
	//	int ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
	//		data, data_size,
	//		AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
	//	if (ret < 0) {
	//		fprintf(stderr, "Error while parsing\n");
	//		exit(1);
	//	}
	//	data += ret;
	//	data_size -= ret;

	//	// 接收解码数据并写入到输出文件
	//	if (pkt->size)
	//		decode(c, pkt, decoded_frame, audioPck->mData);

	//	// 已读数据所剩无几时，读入新一轮的数据
	//	if (data_size < AUDIO_REFILL_THRESH) {
	//		memmove(inbuf, data, data_size);		// 将data剩的数据移动到inbuf开头
	//		data = inbuf;
	//		int len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, f);
	//		if (len > 0)	data_size += len;
	//	}
	//}

	///* 处理最后的数据 */
	//pkt->data = NULL;
	//pkt->size = 0;
	//decode(c, pkt, decoded_frame, audioPck->mData);

	while (av_read_frame(fmt_ctx, pkt) >= 0)
	{
		if (pkt->stream_index == audioStreamIndex)
		{
			decode(c, pkt, decoded_frame, audioPck->mData);
		}
		av_packet_unref(pkt);
	}

	//fclose(f);

	avcodec_free_context(&c);
	//av_parser_close(parser);
	av_frame_free(&decoded_frame);
	av_packet_free(&pkt);

	delete[] inbuf;

	return audioPck;
}

std::shared_ptr<AudioPackage> AudioManager::LoadAudioFile(std::string path)
{
	auto pack = DecodeAudioFile(path.c_str());

	bool b = false;

	return pack;
}
