#include "crystalpch.h"
#include "Application.h"
#include "Crystal/Renderer/Renderer.h"
#include "Input.h"
#include "Keycodes.h"
#include "Crystal/Core/Timestep.h"

#include <GLFW/glfw3.h>

namespace Crystal {

	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CRYSTAL_PROFILE_FUNCTION();

		CRYSTAL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_Window->SetVSync(false);

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();;
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		CRYSTAL_PROFILE_FUNCTION();
		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		CRYSTAL_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		CRYSTAL_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		CRYSTAL_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));


		for (std::vector<Layer*>::iterator it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run() 
	{
		CRYSTAL_PROFILE_FUNCTION();

		while (m_Running)
		{
			CRYSTAL_PROFILE_SCOPE("Run Loop");
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) {
				CRYSTAL_PROFILE_SCOPE("Layerstack OnUpdates");
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			{
				CRYSTAL_PROFILE_SCOPE("Layerstack OnImGuiRender");
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		CRYSTAL_PROFILE_FUNCTION();
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}