#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <iunoplugincontroller.h>
#include <iunoplugin.h>
#include <iunostreamobserver.h>
#include <iunoaudioobserver.h>
#include <iunoaudioprocessor.h>
#include <iunostreamobserver.h>
#include <iunoannotator.h>

#include "SDRunoPlugin_TemplateUi.h"

class SDRunoPlugin_Template : public IUnoPlugin, IUnoStreamProcessor, IUnoAnnotator, IUnoAudioObserver
{

public:
	
	SDRunoPlugin_Template(IUnoPluginController& controller);
	virtual ~SDRunoPlugin_Template();

	// TODO: change the plugin title here
	virtual const char* GetPluginName() const override { return "SDRuno Plugin Example"; }

	// IUnoPlugin
	virtual void HandleEvent(const UnoEvent& ev) override;
	virtual void StreamProcessorProcess(channel_t channel, Complex* buffer, int length, bool& modified) override;
	virtual void AnnotatorProcess(std::vector<IUnoAnnotatorItem>& items) override;
	virtual void AudioObserverProcess(channel_t channel, const float* buffer, int length) override;

	void Register(channel_t ch, bool state)
	{
		if (state)
		{
			m_controller.RegisterAudioObserver(ch, this);
			m_controller.RegisterStreamProcessor(ch, this);
			m_controller.RegisterAnnotator(this);
		}
		else
		{
			m_controller.UnregisterStreamProcessor(ch, this);
			m_controller.UnregisterAnnotator(this);
			m_controller.UnregisterAudioObserver(ch, this);
		}
	}

private:
	
	void WorkerFunction();
	std::thread* m_worker;
	std::mutex m_lock;
	SDRunoPlugin_TemplateUi m_form;
};