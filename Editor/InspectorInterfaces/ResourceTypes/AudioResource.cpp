#include "InspectorResourceTypes.hpp"
#include <Resources/ResourceTypes/AudioClipResource.hpp>
#include <imgui.h>

using namespace VSGE;

void VSGEditor::DrawAudioResourceUI(VSGE::Resource* resource) {
	AudioClipResource* aud_resource = (AudioClipResource*)resource;

	if (!aud_resource->IsReady()) {
		aud_resource->Load();
		return;
	}

	AudioBuffer* buffer = aud_resource->GetAudioBuffer();

	std::string channels_text = "Channels - " + std::to_string(buffer->GetChannels());
	ImGui::Text(channels_text.c_str());

	std::string duration_text = "Duration - " + std::to_string((float)buffer->GetDuration() / 1000.f) + " sec.";
	ImGui::Text(duration_text.c_str());

	std::string bitrate_text = "Bit Rate - " + std::to_string(buffer->GetBitRate()) + " kbit/sec";
	ImGui::Text(bitrate_text.c_str());
}