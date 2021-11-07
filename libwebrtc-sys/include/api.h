#pragma once
#include "libwebrtc-sys/include/webrtc_api.h"
#include "libwebrtc-sys/include/peer_connection_factory.h"
#include "libwebrtc-sys/include/audio_device_module.h"

class ArcasAPI
{
private:
    std::unique_ptr<rtc::Thread> worker_thread;
    std::unique_ptr<rtc::Thread> signaling_thread;
    std::unique_ptr<rtc::Thread> network_thread;

public:
    ArcasAPI() : worker_thread(rtc::Thread::Create()),
                 signaling_thread(rtc::Thread::Create()),
                 network_thread(rtc::Thread::CreateWithSocketServer())
    {
        worker_thread->Start();
        signaling_thread->Start();
        network_thread->Start();
    }

    std::unique_ptr<ArcasPeerConnectionFactory> create_factory() const
    {
        // TODO: Add configuration options for log levels.
        // rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE);
        // rtc::LogMessage::SetLogToStderr(true);

        webrtc::PeerConnectionFactoryDependencies dependencies;
        dependencies.network_thread = network_thread.get();
        dependencies.signaling_thread = signaling_thread.get();
        dependencies.worker_thread = worker_thread.get();
        dependencies.call_factory = webrtc::CreateCallFactory();
        dependencies.task_queue_factory = webrtc::CreateDefaultTaskQueueFactory();
        dependencies.event_log_factory = std::make_unique<webrtc::RtcEventLogFactory>(dependencies.task_queue_factory.get());

        auto adm = rtc::make_ref_counted<ArcasAudioDeviceModule>();

        cricket::MediaEngineDependencies media_deps;
        media_deps.task_queue_factory = dependencies.task_queue_factory.get();
        media_deps.audio_encoder_factory = webrtc::CreateBuiltinAudioEncoderFactory();
        media_deps.audio_decoder_factory = webrtc::CreateBuiltinAudioDecoderFactory();
        media_deps.video_encoder_factory = webrtc::CreateBuiltinVideoEncoderFactory();
        media_deps.video_decoder_factory = webrtc::CreateBuiltinVideoDecoderFactory();
        media_deps.audio_processing = webrtc::AudioProcessingBuilder().Create();
        media_deps.audio_mixer = webrtc::AudioMixerImpl::Create();
        media_deps.adm = adm;

        dependencies.media_engine = cricket::CreateMediaEngine(std::move(media_deps));

        auto factory = webrtc::CreateModularPeerConnectionFactory(std::move(dependencies));

        RTC_LOG(LS_INFO) << ">>>>>>>>>>>>>>>>>>> instantiated PEERCONNECTION_FACTORY";

        return std::make_unique<ArcasPeerConnectionFactory>(factory);
    }

    std::unique_ptr<ArcasPeerConnectionFactory> create_factory_with_arcas_video_encoder_factory(std::unique_ptr<ArcasVideoEncoderFactory> video_encoder_factory) const
    {
        // TODO: Add configuration options for log levels.
        // rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE);
        // rtc::LogMessage::SetLogToStderr(true);

        webrtc::PeerConnectionFactoryDependencies dependencies;
        dependencies.network_thread = network_thread.get();
        dependencies.signaling_thread = signaling_thread.get();
        dependencies.worker_thread = worker_thread.get();
        dependencies.call_factory = webrtc::CreateCallFactory();
        dependencies.task_queue_factory = webrtc::CreateDefaultTaskQueueFactory();
        dependencies.event_log_factory = std::make_unique<webrtc::RtcEventLogFactory>(dependencies.task_queue_factory.get());

        auto adm = rtc::make_ref_counted<ArcasAudioDeviceModule>();

        cricket::MediaEngineDependencies media_deps;
        media_deps.task_queue_factory = dependencies.task_queue_factory.get();
        media_deps.audio_encoder_factory = webrtc::CreateBuiltinAudioEncoderFactory();
        media_deps.audio_decoder_factory = webrtc::CreateBuiltinAudioDecoderFactory();
        media_deps.video_encoder_factory = std::move(video_encoder_factory);
        media_deps.video_decoder_factory = webrtc::CreateBuiltinVideoDecoderFactory();
        media_deps.audio_processing = webrtc::AudioProcessingBuilder().Create();
        media_deps.audio_mixer = webrtc::AudioMixerImpl::Create();
        media_deps.adm = adm;

        dependencies.media_engine = cricket::CreateMediaEngine(std::move(media_deps));

        auto factory = webrtc::CreateModularPeerConnectionFactory(std::move(dependencies));

        RTC_LOG(LS_INFO) << ">>>>>>>>>>>>>>>>>>> instantiated PEERCONNECTION_FACTORY";

        return std::make_unique<ArcasPeerConnectionFactory>(factory);
    }
};

std::unique_ptr<ArcasAPI> create_arcas_api();
