#include <sstream>
#include <unoevent.h>
#include <iunoplugincontroller.h>
#include <vector>
#include <sstream>
#include <chrono>

#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"

SDRunoPlugin_Template::SDRunoPlugin_Template(IUnoPluginController& controller) :
	IUnoPlugin(controller),
	m_form(*this, controller),
	m_worker(nullptr)
{
}

SDRunoPlugin_Template::~SDRunoPlugin_Template()
{	
}

void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev)
{
	m_form.HandleEvent(ev);	
}

void SDRunoPlugin_Template::StreamProcessorProcess(channel_t channel, Complex* buffer, int length, bool& modified)
{
	m_form.StreamProcessorProcess(channel, buffer, length, modified);
};

void SDRunoPlugin_Template::AnnotatorProcess(std::vector<IUnoAnnotatorItem>& items)
{
	m_form.AnnotatorProcess(items);
};

void SDRunoPlugin_Template::AudioObserverProcess(channel_t channel, const float* buffer, int length)
{
	m_form.AudioObserverProcess(channel, buffer, length);
};

void SDRunoPlugin_Template::WorkerFunction()
{
	// Worker Function Code Goes Here
}