#include "OpenGL_RAPI.hpp"
#include <Core/Logger.hpp>

#include <GL/glew.h>

VSGE::OpenGL_RAPI* VSGE::OpenGL_RAPI::_this = nullptr;

void VSGE::OpenGL_RAPI::Create(Window* window) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //OpenGL 4.5
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    mGlContext = SDL_GL_CreateContext(window->GetSdlWinPtr());

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        Logger::Log(LogType::LOG_TYPE_ERROR) << "GLEW initialize failed\n";
    else
        Logger::Log(LogType::LOG_TYPE_INFO) << "GLEW initialize sucessful\n";

}

void VSGE::OpenGL_RAPI::Destroy() {
    SDL_GL_DeleteContext(mGlContext);
}