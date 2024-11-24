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
		size_t dataLen = pack->mData.size();
		size_t currIndex = pack->mPlaybackProgress;

		// �����ǰ��Ƶ�����Ѿ�������ϣ��Ƿ�ѭ������
		if (currIndex >= dataLen)
			if (pack->mIsLoop == false)
			{
				it = packages.erase(it);	// ��������Ҳ�ѭ�����޳�
				continue;
			}
			else
				pack->mPlaybackProgress = 0;

		// ʣ���ֽ��Ƿ�С�ڻ�������С
		size_t remainingLen = dataLen - currIndex;
		size_t fillLen = len < remainingLen ? len : remainingLen;

		// ������Ƶ����װ�ص���Ƶ��������
		SDL_MixAudio(stream, pack->mData.data() + currIndex, fillLen,
			SDL_MIX_MAXVOLUME * pack->mVolume);

		// ��Ƶָ������
		currIndex += len;
		pack->mPlaybackProgress += fillLen;

		// ��ʾ��ǰ��Ƶ�Ľ���
		//SDL_Log(u8"���Ž��ȣ�%d", pack->mPlaybackProgress);

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
	std::memcpy(pack->mData.data(), buf, len);

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

	// �ⲿ��ԭ���ݽ�����֮�󣬴������ݰ�

	// �����ݰ�������������MP3��Ƶ����->������->PCM��Ƶ����
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return;
	}

	// �ӱ�����������������֡�����ս���������֡
	while (ret >= 0) {
		ret = avcodec_receive_frame(dec_ctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}

		// �ӽ������л�ȡ������ʽ�������ֽ� ÿ�� �������޷����з��ţ�8λ16λ����������������
		 int data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
		if (data_size < 0) {
			/* ��Ӧ�ó��ֵ���� */
			fprintf(stderr, "Failed to calculate data size\n");
			exit(1);
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

std::shared_ptr<AudioPackage> DecodeAudioFile(const char* intputFile)
{
	// FFmpeg��UTF8֧����ã�����GBK·������Ҳ�ܶ����ļ�������·�������Ļ�����
	// ���Ƕ�ȡ�ļ��ڵ����Ĳ�������

	FILE* f;
	uint8_t* inbuf = new uint8_t[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
	AVFrame* decoded_frame = NULL;

	//av_register_all();
	// ���ļ��ж�����ʽ
	AVFormatContext* fmt_ctx = avformat_alloc_context();
	int res = avformat_open_input(&fmt_ctx, intputFile, NULL, NULL);
	res = avformat_find_stream_info(fmt_ctx, NULL);		// ��ȡ�ļ�������Щ��
	av_dump_format(fmt_ctx, 0, intputFile, 0);		// �����Ϣ

	// Ѱ�����е���Ƶ������һ���ҵ�����Ƶ��
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

	// ������Ϸ����Ƶ������������Ƶ����
	auto audioPck = std::make_shared<AudioPackage>();
	//SDL_memset(&audioPck->mDesiredSpec, 0, sizeof(SDL_AudioSpec));
	audioPck->mDesiredSpec.freq = pParameters->sample_rate;
	audioPck->mDesiredSpec.format = AVSampleFormatToSDLAudioFormat((AVSampleFormat)pParameters->format);
	audioPck->mDesiredSpec.channels = pParameters->channels;

	// �������ݰ�
	AVPacket* pkt = av_packet_alloc();

	// ����mp3������
	const AVCodec* codec = avcodec_find_decoder(pParameters->codec_id);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}

	// ���ݽ�����ID���ļ���ʽ������ʼ��������������
	//AVCodecParserContext* parser = av_parser_init(codec->id);
	//if (!parser) {
	//	fprintf(stderr, "Parser not found\n");
	//	exit(1);
	//}

	// ���䣬������ ������
	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c) {
		fprintf(stderr, "Could not allocate audio codec context\n");
		exit(1);
	}

	// �� �ļ��Ĳ��� ���Ƶ� ������������
	avcodec_parameters_to_context(c, pParameters);
	c->channel_layout = AV_CH_LAYOUT_STEREO;

	// �򿪽�����
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		exit(1);
	}
	
	//	// ֻ�ڵ�һ��ѭ������������� �� ֡
	if (!(decoded_frame = av_frame_alloc())) {
		fprintf(stderr, "Could not allocate audio frame\n");
		exit(1);
	}

	// ����������ļ�
	//fopen_s(&f, intputFile, "rb");		// ������·����GBK����
	//if (!f) {
	//	fprintf(stderr, "Could not open %s\n", intputFile);
	//	exit(1);
	//}

	// �״ζ�ȡԭ��Ƶ����
	//uint8_t* data = inbuf;
	//size_t data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);

	//while (data_size > 0) {

	//	// ֻ�ڵ�һ��ѭ������������� �� ֡
	//	if (!decoded_frame) {
	//		if (!(decoded_frame = av_frame_alloc())) {
	//			fprintf(stderr, "Could not allocate audio frame\n");
	//			exit(1);
	//		}
	//	}

	//	// ԭ�ļ�����->������->MP3�������ݣ�����һ��ԭʼ���� �����ݰ�
	//	int ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
	//		data, data_size,
	//		AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
	//	if (ret < 0) {
	//		fprintf(stderr, "Error while parsing\n");
	//		exit(1);
	//	}
	//	data += ret;
	//	data_size -= ret;

	//	// ���ս������ݲ�д�뵽����ļ�
	//	if (pkt->size)
	//		decode(c, pkt, decoded_frame, audioPck->mData);

	//	// �Ѷ�������ʣ�޼�ʱ��������һ�ֵ�����
	//	if (data_size < AUDIO_REFILL_THRESH) {
	//		memmove(inbuf, data, data_size);		// ��dataʣ�������ƶ���inbuf��ͷ
	//		data = inbuf;
	//		int len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, f);
	//		if (len > 0)	data_size += len;
	//	}
	//}

	///* ������������ */
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
