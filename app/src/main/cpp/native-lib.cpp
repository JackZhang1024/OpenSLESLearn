#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
//#include <malloc.h>
#include <android/log.h>

#define TAG "NDK_LEARN"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

void initCreateOpenSLES();

FILE *pcmFile;
void *pcmBuffer;

// 函数指针
void playerCallback(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
    // 回调函数 循环体
    if (!feof(pcmFile)) {
        // 每次只读一个字节  读多少 44100*2*2 fread(pcmBuffer, 1, 44100 * 2 * 2, pcmFile);
        fread(pcmBuffer, 1, 16000 * 2 * 2, pcmFile);
        //(*caller)->Enqueue(caller, pcmBuffer, 44100 * 2 * 2);
        (*caller)->Enqueue(caller, pcmBuffer, 16000 * 2 * 2);
    } else {
        fclose(pcmFile);
        free(pcmBuffer);
    }
}


void initCreateOpenSLES() {
    // 1. 创建引擎接口对象
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;
    // create engine
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    // realize the engine
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    // get the engine interface, which is needed in order to create other objects
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    static SLObjectItf outputMixObject = NULL;
    //SLuint32 numInterfaces; // 混音器的个数
    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};

    // 2. 设置混音器
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    // realize the output mix
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                     &outputMixEnvironmentalReverb);
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb,
                                                                      &reverbSettings);
    // 3. 创建播放器
    SLObjectItf pPlayer = NULL;
    SLPlayItf pPlayItf = NULL;
    /// 播放器能播什么？？
    SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM,
            2,
            //SL_SAMPLINGRATE_44_1, // 采样率 44.1K 声音短促 速度快
            SL_SAMPLINGRATE_16, // 采样率 16K 声音正常 速度正常
            //SL_SAMPLINGRATE_12, // 采样率 12K 声音低沉 播放速度慢
            SL_PCMSAMPLEFORMAT_FIXED_16,  // 2字节
            SL_PCMSAMPLEFORMAT_FIXED_16,  // 2字节
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&simpleBufferQueue, &formatPcm};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, NULL};

    // SL_IID_PLAYBACKRATE 声音变速
    SLInterfaceID interfaceIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAYBACKRATE};
    SLboolean interfaceRequired[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pPlayer, &audioSrc, &audioSnk, 3,
                                       interfaceIds, interfaceRequired);
    // realize the engine
    (*pPlayer)->Realize(pPlayer, SL_BOOLEAN_FALSE);
    // get the engine interface, which is needed in order to create other objects
    (*pPlayer)->GetInterface(pPlayer, SL_IID_PLAY, &pPlayItf);
    // 4. 设置缓存队列和回调函数
    // get the buffer queue interface
    SLAndroidSimpleBufferQueueItf playerBufferQueue;
    (*pPlayer)->GetInterface(pPlayer, SL_IID_BUFFERQUEUE, &playerBufferQueue);
    // register callback on the buffer queue
    (*playerBufferQueue)->RegisterCallback(playerBufferQueue, playerCallback, NULL);
    // 5. 设置播放状态 Itf 接口
    (*pPlayItf)->SetPlayState(pPlayItf, SL_PLAYSTATE_PLAYING);
    // 6. 调用回调函数
    playerCallback(playerBufferQueue, NULL);

}

// 16K采样的PCM文件 就应该是16K的采样频率来重新采样和播放
extern "C"
JNIEXPORT void JNICALL
Java_com_luckyboy_opensleslearn_MainActivity_playPCM(JNIEnv *env, jobject thiz,
                                                     jstring music_path_) {
    const char *music_path = env->GetStringUTFChars(music_path_, 0);
    pcmFile = fopen(music_path, "r");
    //pcmBuffer = malloc(44100 * 2 * 2);
    pcmBuffer = malloc(16000 * 2 * 2); //创建缓冲区
    initCreateOpenSLES();
    env->ReleaseStringUTFChars(music_path_, music_path);
}
