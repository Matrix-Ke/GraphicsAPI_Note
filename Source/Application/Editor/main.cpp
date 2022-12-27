#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#include "Engine.h"
#include "Editor/Editor.h"



int main(int argc, char** argv)
{
	std::filesystem::path executable_path(argv[0]);
	std::filesystem::path config_file_path = executable_path.parent_path() / "PiccoloEditor.ini";

	Matrix::MatrixEngine* engine = new Matrix::MatrixEngine();

	engine->startEngine(config_file_path.generic_string());

	engine->initialize();

	Matrix::MatrixEditor* editor = new Matrix::MatrixEditor();
	editor->initialize(engine);

	editor->run();

	editor->clear();

	engine->clear();
	engine->shutdownEngine();

	return 0;
}
