#pragma once
#include "rust/cxx.h"
#include "libwebrtc-sys/include/webrtc_api.h"
#include "libwebrtc-sys/include/peer_connection_factory.h"

rust::String session_description_to_string(const webrtc::SessionDescriptionInterface &sdp);
