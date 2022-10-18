#pragma once

#include <memory>

namespace Matrix
{
	class EditorUI;
	class MatrixEngine;

	class MatrixEditor
	{
	public:
		MatrixEditor();
		virtual ~MatrixEditor();

	public:

		void initialize(MatrixEngine* engine_runtime);
		void clear();
		void run();



	private:
		std::shared_ptr<EditorUI> mPtrEditorUi;
		MatrixEngine* mPtrEngineRuntime{ nullptr };
	};
}