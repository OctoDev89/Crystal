#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "Crystal/LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "Crystal/ImGui/ImGuiLayer.h"
#include "Crystal/Renderer/Shader.h"
#include "Crystal/Renderer/Buffer.h"
#include "Crystal/Renderer/VertexArray.h"
#include "Crystal/Renderer/OrthographicCamera.h"


namespace Crystal {
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow(){ return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}