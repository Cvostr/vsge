#include "GLFramebuffer.hpp"

using namespace VSGE;

void GLFramebuffer::AddAttachment(Texture* attachment) {
	mAttachments.push_back(attachment);
}

void GLFramebuffer::AddAttachment(TextureFormat Format, uint32 layers) {

}

void GLFramebuffer::AddDepth(TextureFormat Format, uint32 Layers) {

}

bool GLFramebuffer::Create() {
	return false;
}