#include <sstream>
#include <fstream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <unoevent.h>

#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateForm.h"

// Ui constructor - load the Ui control into a thread
SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller) :
	m_parent(parent),
	m_form(nullptr),
	m_controller(controller),
	m_started(false)
{
	m_logfile.open("C:\\temp\\unolog.txt");
	m_logfile << "Ui Constructor" << std::endl;
	m_iqfile.open("C:\\temp\\unoiq.bin", std::ios::binary | std::ios::out);

	m_thread = std::thread(&SDRunoPlugin_TemplateUi::ShowUi, this);
}

// Ui destructor (the nana::API::exit_all();) is required if using Nana UI library
SDRunoPlugin_TemplateUi::~SDRunoPlugin_TemplateUi()
{	
	nana::API::exit_all();
	m_thread.join();	
	m_logfile.close();
	m_iqfile.close();
}

// Show and execute the form
void SDRunoPlugin_TemplateUi::ShowUi()
{	
	m_lock.lock();
	m_form = std::make_shared<SDRunoPlugin_TemplateForm>(*this, m_controller);
	m_lock.unlock();

	m_logfile << "ShowUi::Run" << std::endl;
	m_form->Run();
}

// Load X from the ini file (if exists)
// TODO: Change Template to plugin name
int SDRunoPlugin_TemplateUi::LoadX()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Template.X", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	m_logfile << "LoadX: " << tmp << std::endl;
	return stoi(tmp);
}

// Load Y from the ini file (if exists)
// TODO: Change Template to plugin name
int SDRunoPlugin_TemplateUi::LoadY()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Template.Y", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	m_logfile << "LoadY: " << tmp << std::endl;
	return stoi(tmp);
}

// Handle events from SDRuno
// TODO: code what to do when receiving relevant events
void SDRunoPlugin_TemplateUi::HandleEvent(const UnoEvent& ev)
{
	std::ofstream::sentry s(m_logfile);
	
	m_logfile << "HandleEvent::" << ev.ToString() << " (" << ev.GetChannel() << ") : ";
	switch (ev.GetType())
	{
	case UnoEvent::DemodulatorChanged:
		m_logfile << m_controller.GetDemodulatorType(ev.GetChannel());
		break;
	case UnoEvent::BandwidthChanged:
		m_logfile << m_controller.GetFilterBandwidth(ev.GetChannel());
		break;
	case UnoEvent::FrequencyChanged:
		m_logfile << m_controller.GetVfoFrequency(ev.GetChannel());
		break;
	case UnoEvent::CenterFrequencyChanged:
		m_logfile << m_controller.GetCenterFrequency(ev.GetChannel());
		break;
	case UnoEvent::SampleRateChanged:
		m_logfile << m_controller.GetSampleRate(ev.GetChannel());
		break;
	case UnoEvent::StreamingStarted:
		break;
	case UnoEvent::StreamingStopped:
		break;
	case UnoEvent::SquelchEnableChanged:
		m_logfile << m_controller.GetSquelchEnable(ev.GetChannel());
		break;
	case UnoEvent::SquelchThresholdChanged:
		m_logfile << m_controller.GetSquelchLevel(ev.GetChannel());
		break;
	case UnoEvent::AgcThresholdChanged:
		m_logfile << m_controller.GetAgcThreshold(ev.GetChannel());
		break;
	case UnoEvent::AgcModeChanged:
		m_logfile << m_controller.GetAgcMode(ev.GetChannel());
		break;
	case UnoEvent::NoiseBlankerLevelChanged:
		m_logfile << m_controller.GetNoiseBlankerLevel(ev.GetChannel());
		break;
	case UnoEvent::NoiseReductionLevelChanged:
		m_logfile << m_controller.GetNoiseReductionLevel(ev.GetChannel());
		break;
	case UnoEvent::CwPeakFilterThresholdChanged:
		m_logfile << m_controller.GetCwPeakFilterThreshold(ev.GetChannel());
		break;
	case UnoEvent::FmNoiseReductionEnabledChanged:
		m_logfile << m_controller.GetFmNoiseReductionEnable(ev.GetChannel());
		break;
	case UnoEvent::FmNoiseReductionThresholdChanged:
		m_logfile << m_controller.GetFmNoiseReductionThreshold(ev.GetChannel());
		break;
	case UnoEvent::WfmDeemphasisModeChanged:
		m_logfile << m_controller.GetWfmDeemphasisMode(ev.GetChannel());
		break;
	case UnoEvent::AudioVolumeChanged:
		m_logfile << m_controller.GetAudioVolume(ev.GetChannel());
		break;
	case UnoEvent::AudioMuteChanged:
		m_logfile << m_controller.GetAudioMute(ev.GetChannel());
		break;
	case UnoEvent::IFGainChanged:
		break;
	case UnoEvent::SavingWorkspace:
		break;
	default:
		break;
	}

	m_logfile << std::endl;
}

// Required to make sure the plugin is correctly unloaded when closed
void SDRunoPlugin_TemplateUi::FormClosed()
{
	m_controller.RequestUnload(&m_parent);
}


void SDRunoPlugin_TemplateUi::StreamProcessorProcess(channel_t channel, Complex* buffer, int length, bool& modified)
{
	std::ofstream::sentry s(m_logfile);
	m_logfile << "StreamObserver: " << "(" << channel << ") : " << length << std::endl;
	m_iqfile.write(reinterpret_cast<const char *>(buffer), length * sizeof(Complex));
	modified = false;
}


void SDRunoPlugin_TemplateUi::AnnotatorProcess(std::vector<IUnoAnnotatorItem>& items)
{
	std::ofstream::sentry s(m_logfile);
	m_logfile << "Annotator: " << "(" << items.size() << ")" << std::endl;

	if (items.size() == 0)
	{
		IUnoAnnotatorItem a;
		a.frequency = 60000;
		a.power = -85;
		a.rgb = 0xFFFFFF;
		a.style = AnnotatorStyleFlag;
		a.text = "[15:43 MSF]";

		items.push_back(a);
	}

}

void SDRunoPlugin_TemplateUi::AudioObserverProcess(channel_t channel, const float* buffer, int length)
{
	std::ofstream::sentry s(m_logfile);
	m_logfile << "AudioObserver: " << "(" << channel << ") : " << length << std::endl;
}


void SDRunoPlugin_TemplateUi::Toggle(void)
{
	std::ofstream::sentry s(m_logfile);
	channel_t ch = 0;

	if (m_started == false)
	{
		m_logfile << "Toggle -> ON: " << std::endl;
		
		m_logfile << "VF : " << m_controller.SetVfoFrequency(ch, 70000) << " (" << m_controller.GetVfoFrequency(ch) << ")" << std::endl;
		m_logfile << "CF : " << m_controller.SetCenterFrequency(ch, 60000) << " (" << m_controller.GetCenterFrequency(ch) << ")" << std::endl;
		m_logfile << "AM : " << m_controller.SetAudioMute(ch, true) << " (" << m_controller.GetAudioMute(ch) << ")" << std::endl;
		m_logfile << "AGC: " << m_controller.SetAgcMode(ch, IUnoPluginController::AgcMode::AGCModeMedium) << " (" << (int)m_controller.GetAgcMode(ch) << ")" << std::endl;
		m_logfile << "DM : " << m_controller.SetDemodulatorType(ch, IUnoPluginController::DemodulatorType::DemodulatorCW) << " (" << (int)m_controller.GetDemodulatorType(ch) << ")" << std::endl;
		m_logfile << "BW : " << m_controller.SetFilterBandwidth(ch, 150) << " (" << m_controller.GetFilterBandwidth(ch) << ")" << std::endl;


		m_logfile << "SR :" << m_controller.SetSampleRate(ch, 1000000.0) << " (" << m_controller.GetSampleRate(ch) << ")" << std::endl;
		m_logfile << "SQ :" << m_controller.SetSquelchEnable(ch, false) << " (" << m_controller.GetSquelchEnable(ch) << ")" << std::endl;
	
		m_controller.RegisterAudioObserver(ch, this);
		m_controller.RegisterStreamProcessor(ch, this);
		m_controller.RegisterAnnotator(this);
	}
	else
	{
		m_logfile << "Toggle -> OFF: " << std::endl;
		m_controller.UnregisterStreamProcessor(ch, this);
		m_controller.UnregisterAnnotator(this);
		m_controller.UnregisterAudioObserver(ch, this);
	}

	m_started = !m_started;

}