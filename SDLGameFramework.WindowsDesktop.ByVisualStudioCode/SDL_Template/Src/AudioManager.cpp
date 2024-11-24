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
	//�ص������ֶ�������Ƶ��
	SDL_memset(stream, 0, len);

	auto ins = (AudioManager*)userdata;
	auto& packages = ins->mAudioPackages;

	std::lock_guard<std::mutex> lock(ins->mMutex);

	for (auto it = packages.begin(); it != packages.end();)
	{
		auto& pack_wp = (*it);
		// �޳�ʧЧ��Ƶ����
		if (pack_wp.expired() == true)
		{
			it = packages.erase(it);
			continue;
		}

		auto pack = pack_wp.lock();
		// ����Ƶ���Ƿ�������
		if (pack->mIsPlaying == true)
		{
			size_t dataLen = pack->mData.size();
			size_t currIndex = pack->mPlaybackProgress;

			// �����ǰ��Ƶ�����Ѿ�������ϣ��Ƿ�ѭ������
			if (currIndex >= dataLen)
				if (pack->mIsLoop == false)
				{
					pack->mIsPlaying = false;
					it = packages.erase(it);	// ��������Ҳ�ѭ�����޳�
					continue;
				}
				else
					pack->mPlaybackProgress = 0;

			// ʣ���ֽ��Ƿ�С�ڻ�������С
			size_t fillLen = std::min<size_t>(len, dataLen - currIndex);
			pack->mPlaybackProgress += fillLen;
			// ������Ƶ����װ�ص���Ƶ��������
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

	// ���������Ĳ��Ų���
	ins.mDesiredSpec.freq = 44100;	// ������ ÿ��
	ins.mDesiredSpec.format = AUDIO_S16;	//������ʽ��Ĭ��PCM�з���ʽ | λ��
	ins.mDesiredSpec.channels = 2;		// ����
	ins.mDesiredSpec.silence = 0;	// ����ֵ����������Ƶ���ݵ�ʱ���ȡֵ��
	ins.mDesiredSpec.samples = 1024;	// ÿͨ�����������ٴ�С������ռ�ռ䣬С��Ƶ���ص�
	// size = samples * channels * (bits per sample / 8)
	ins.mDesiredSpec.callback = AudioCallback;
	ins.mDesiredSpec.userdata = &ins;		//����ǰ������Ϊ�û����ݴ���

	// ������ڶ���������ֵ������Ҫ�ֶ�ת�룬����û������ 
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
	SDL_CloseAudio();	// ����־��棬δ֪
}

void AudioManager::Play(std::weak_ptr<AudioPackage> pack_wp)
{
	auto& ins = GetInstance();
	auto& packs = ins.mAudioPackages;
	auto& desiredSpec = ins.mDesiredSpec;
	auto pack = pack_wp.lock();

	if (!pack) return;

	//auto it = std::find(packs.begin(), packs.end(), pack);
	//// �Ѿ����ڣ���Ҫ�ظ����
	//if (it != packs.end())
	//	return;

	// δ���ڣ������Ƶ������
	// �Ƚ���Ƶ�Ͳ�������ʽ������ͬ����Ҫת��
	if (pack->mDesiredSpec.freq != desiredSpec.freq ||
		pack->mDesiredSpec.format != desiredSpec.format ||
		pack->mDesiredSpec.channels != desiredSpec.channels)
	{
		SDL_AudioCVT cvt;
		int res = SDL_BuildAudioCVT(&cvt,
			pack->mDesiredSpec.format, pack->mDesiredSpec.channels, pack->mDesiredSpec.freq,
			desiredSpec.format, desiredSpec.channels, desiredSpec.freq);

		SDL_assert(cvt.needed);
		cvt.len = pack->mData.size();		// Դ���ݴ�С
		pack->mData.resize(cvt.len * cvt.len_mult);	// Ԥ����������С = ԭ��С * �������
		cvt.buf = pack->mData.data();	// ���뻺���������Ƚ�ԭ���ݸ��Ƶ���������ת��������ݽ��Ḳ��
		cvt.len_cvt;

		int res2 = SDL_ConvertAudio(&cvt);

		// ��ȡת����ɺ��С��Ҫ����cvt.len * cvt.len_ratio��cvt.len_ratio�����ֵ��ֱ��ʹ��cvt.len_cvt����
		pack->mData.resize(cvt.len_cvt);	// ת�������Ч�ֽ��� = ԭ��С * ���ű���
		pack->mDesiredSpec = desiredSpec;
	}

	// ��ӵ���Ƶ������
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
	// ����WAV��Ƶ�ļ���Ϣ������
	SDL_AudioSpec audioSpec;
	uint8_t* buf = nullptr;
	uint32_t len = 0;
	auto res = SDL_LoadWAV(path.c_str(), &audioSpec, &buf, &len);

	// ��SDL��Ƶ���ݴ��ΪAudioPackage
	if (res == NULL)
	{
		SDL_Log("WAV��Ƶ����ʧ�ܣ�%s", path.c_str());
		return nullptr;
	}

	auto pack = std::make_shared<AudioPackage>();
	pack->mDesiredSpec = audioSpec;
	pack->mData.resize(len);
	memcpy(pack->mData.data(), buf, len);

	// �ͷ�WAV���ص���Դ
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

/*���·�װFFMpeg��һЩ���󣬷���RAII�Զ��ͷ��ڴ�*/ 

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


// �� FFMpeg����Ƶ��ʽ�� תΪ SDL��Ƶ��ʽ��
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

// ��Ƶ���벿��
void AudioDecode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, std::vector<uint8_t>& outData)
{
	if (!dec_ctx || !pkt || !frame)
		return;

	int ret;

	// �ⲿ��ԭ���ݽ�����֮�󣬴������ݰ�

	// �����ݰ�������������MP3��Ƶ����->������->PCM��Ƶ����
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) 
	{
		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return;
	}

	// �ӱ�����������������֡�����ս���������֡
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

		// �ӽ������л�ȡ������ʽ�������ֽ� ÿ�� �������޷����з��ţ�8λ16λ����������������
		 int data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
		if (data_size < 0) 
		{
			fprintf(stderr, "Failed to calculate data size\n");
			return;
		}
		
		// ����֡��ʵ���м���ͨ��������
		int frameChannels = 0;
		for (; frameChannels < AV_NUM_DATA_POINTERS; frameChannels++)
			if (frame->data[frameChannels] == nullptr) break;

		// ���ݽ�������
		auto iOutData = outData.size();	// �ϴ���ʹ�õ��ֽ���
		auto iThisBytes = frame->nb_samples * data_size * dec_ctx->channels;		// ���������ֽ���
		outData.resize(outData.size() + iThisBytes);	// resize���ݺ�sizeҲ�����ӣ���reserveֻ�����ݲ�Ӱ��size
		auto pOutData = outData.data() + iOutData;	// ��ȡ�����ڲ�ָ�룬���ƶ������β����ʼλ��
		
		// ������������д���ļ�
		if (frameChannels == 1)	// ������ �� ��ƽ�д洢ʱ�����������ֻ����data[0]��ֱ�Ӱ���
			memcpy(pOutData, frame->data[0], iThisBytes);
		else
			for (int si = 0; si < frame->nb_samples; si++)
				for (int ch = 0; ch < dec_ctx->channels; ch++)
					for (int bi = 0; bi < data_size; bi++)
						*(pOutData++) = frame->data[ch][data_size * si + bi];	//���������emplace_backЧ���������ˣ�ֱ��д���ڲ�����ǳ���
	}
}

// ͼƬ���벿��
void ImageDecode(AVCodecContext* dec_ctx, AVPacket* pkt, AVFrame* frame, Texture& outData)
{
	if (!dec_ctx || !pkt || !frame || !outData)
		return;

	int ret;

	// �ⲿ��ԭ���ݽ�����֮�󣬴������ݰ�

	// �����ݰ�������������MP3��Ƶ����->������->PCM��Ƶ����
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0)
	{
		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return;
	}

	// �ӱ�����������������֡�����ս���������֡
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

		// �ӽ������л�ȡ������ʽ�������ֽ� ÿ�� �������޷����з��ţ�8λ16λ����������������
		int data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
		if (data_size < 0)
		{
			fprintf(stderr, "Failed to calculate data size\n");
			return;
		}

		// ����֡��ʵ���м���ͨ��������
		int frameChannels = 0;
		for (; frameChannels < AV_NUM_DATA_POINTERS; frameChannels++)
			if (frame->data[frameChannels] == nullptr) break;

		SwsContext* swsCtx = sws_getContext(
			dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt, 
			dec_ctx->width, dec_ctx->height, AV_PIX_FMT_ABGR, SWS_BICUBIC, NULL, NULL, NULL);
		
		// ע�⣺SDL_RGBA = FFMpeg_ABGR���෴�ģ�����Ϊ���ߵĸ�ʽ���ƾ��в�ͬ���壺
		// SDL_RGBA�Ǳ�ʾ�ֽڸߵ͹�ϵ��R�ڸ��ֽڣ�A�ڵ��ֽڣ�
		// FFMpeg_ABGR�Ǳ�ʾ�ڴ��ַ�ߵ͹�ϵ��A|B|G|R|A|B|G|R...��A�ڵ͵�ַ��R�ڸߵ�ַ
		// ��Windows��Linuxϵͳ��С�˴洢ϵͳ�����ֽ��ڵ͵�ַ�����ֽ��ڸߵ�ַ��SDL_RGBA = FFMpeg_ABGR
		// �ڴ�˴洢ϵͳ�����ֽ��ڸߵ�ַ�����ֽ��ڵ͵�ַ��SDL_RGBA = FFMpeg_RGBA
		// Ԫ�ش��滹��Ϊ�����ƽ�����֣�������RGB|RGB|RGB��ƽ����RRR...GGG...BBB...
		// ��ƽ��ʱ����Ҫע���С�����⣬����SDL_RGB24 = FFMpeg_RGB24
		// SDL����һ���ӣ�SDL_PIXELFORMAT_BGR888 = SDL_PIXELFORMAT_XBGR8888 ��ʵ��ռ4���ֽ�

		uint8_t* pointers[4];
		int linesizes[4];
		av_image_alloc(pointers, linesizes, dec_ctx->width, dec_ctx->height, AV_PIX_FMT_ABGR, 1);
		sws_scale(swsCtx, frame->data, frame->linesize, 0, frame->height, pointers, linesizes);

		// ������������д���ļ�
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

// ������Ƶ�ļ�·�������ؽ�������Ƶ���ݰ�
std::shared_ptr<AudioPackage> DecodeAudioFile(const char* intputFile)
{
	Texture coverImage;

	// FFmpeg��UTF8֧����ã�����GBK·������Ҳ�ܶ����ļ�������·�������Ļ�����
	// ���Ƕ�ȡ�ļ��ڵ����Ĳ�������

	// ���ļ��ж�����ʽ
	AVFormatContext_ptr formatContext;
	if (formatContext.OpenInput(intputFile, NULL, NULL) != 0)	// ���ļ�
	{
		fprintf(stderr, "Could not open file\n");
		return nullptr;
	}
	if (formatContext.FindStreamInfo(NULL) != 0)		// ��ȡ�ļ�������Щ��
	{
		fprintf(stderr, "Could not find stream info\n");
		return nullptr;
	}
	formatContext.DumpFormat(0, intputFile, 0);		// �����Ϣ

	// Ѱ�����е���Ƶ������һ���ҵ�����Ƶ��
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

	// ����mp3������
	const AVCodec* codec = avcodec_find_decoder(pParameters->codec_id);
	if (codec == nullptr) 
	{
		fprintf(stderr, "Codec not found\n");
		return nullptr;
	}

	// ���䣬������ ������
	AVCodecContext_ptr codecContext(codec);
	if (*codecContext == nullptr) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		return nullptr;
	}

	// �� �ļ��Ĳ��� ���Ƶ� ������������
	codecContext.ParametersToContext(pParameters);

	// �򿪽�����
	if (codecContext.Open2(codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return nullptr;
	}
	
	//	// ֻ�ڵ�һ��ѭ������������� �� ֡
	AVFrame_ptr decodedFrame;

	// �������ݰ�
	AVPacket_ptr packet;

	// ������Ϸ����Ƶ������������Ƶ����
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
