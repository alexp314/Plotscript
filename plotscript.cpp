#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <complex>
//#include <notebook.cpp>
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include <environment.hpp>
#include <message_queue.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <signal.h>
#include <csignal>
//#include "message_queue.hpp"

// This is an example of how to to trap Cntrl-C in a cross-platform manner
// it creates a simple REPL event loop and shows how to interrupt it.

//#include <csignal>
#include <cstdlib>

// This global is needed for communication between the signal handler
// and the rest of the code. This atomic integer counts the number of times
// Cntl-C has been pressed by not reset by the REPL code.
volatile sig_atomic_t global_status_flag = 0;

// *****************************************************************************
// install a signal handler for Cntl-C on Windows
// *****************************************************************************
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>

// this function is called when a signal is sent to the process
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {

	switch (fdwCtrlType) {
	case CTRL_C_EVENT: // handle Cnrtl-C
					   // if not reset since last call, exit
		if (global_status_flag > 0) {
			exit(EXIT_FAILURE);
		}
		++global_status_flag;
		return TRUE;

	default:
		return FALSE;
	}
}

// install the signal handler
inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }
// *****************************************************************************

// *****************************************************************************
// install a signal handler for Cntl-C on Unix/Posix
// *****************************************************************************
#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||             \
    defined(__posix)
#include <unistd.h>

// this function is called when a signal is sent to the process
void interrupt_handler(int signal_num) {

	if (signal_num == SIGINT) { // handle Cnrtl-C
								// if not reset since last call, exit
		if (global_status_flag > 0) {
			exit(EXIT_FAILURE);
		}
		++global_status_flag;
	}
}

// install the signal handler
inline void install_handler() {

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = interrupt_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif

struct outputq
{
	std::string str;
	Expression exp;
	bool type;
	//SemanticError ex;

	//void getType()

};

//typedef message_queue<std::string>;
typedef MessageQueue<std::string> InputQueue;
typedef MessageQueue<outputq> OututQueue;

int repl(Interpreter *interp, Interpreter copy_interp);
void thread_eval(Interpreter *interp, InputQueue *inq, OututQueue *outq);

void prompt() {
	std::cout << "\nplotscript> ";
}

std::string readline() {
	std::string line;
	std::getline(std::cin, line);

	return line;
}

void error(const std::string & err_str) {
	std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str) {
	std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, std::string filename) {

	Interpreter interp;

	if (!interp.parseStream(stream)) {
		error("Invalid Program. Could not parse.");
		return EXIT_FAILURE;
	}
	else {
		try {
			Expression exp = interp.evaluate();
			std::cout << exp << std::endl;
		}
		catch (const SemanticError & ex) {
			std::cerr << ex.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	if (filename == STARTUP_FILE)
	{
		Interpreter copy_interp = interp;
		return repl(&interp, copy_interp);
	
	}
	else 
		return EXIT_SUCCESS;

}


int eval_from_file(std::string filename) {

	std::ifstream ifs(filename);

	if (!ifs) {
		error("Could not open file for reading.");
		return EXIT_FAILURE;
	}

	//repl();


	return eval_from_stream(ifs, filename);
}

int eval_from_command(std::string argexp) {

	std::istringstream expression(argexp);

	return eval_from_stream(expression, argexp);
}

// A REPL is a repeated read-eval-print loop
int repl(Interpreter *interp, Interpreter copy_interp) {
	//Interpreter interp;
	install_handler();
	static bool thread_started = true;
	*interp = copy_interp;
	Expression exp;
	std::string out;
	InputQueue* inq = new InputQueue;
	OututQueue* outq = new OututQueue;
	outputq *outType = new outputq;
	std::thread * thread = new std::thread(thread_eval, interp, inq, outq);

	//std::string in;
	
	
	
	//TODO message queue here

	//
	while (!std::cin.eof()) {
		global_status_flag = 0;


		prompt();
		std::string line = readline();
		if (line.empty()) continue;
		if (line == "%start" && !thread_started)
		{
			thread = new std::thread(thread_eval, interp, inq, outq);
			thread_started = true;
		}
		else if (thread_started && line == "%stop")
		{
			std::string stop_thread = "stop";
			inq->push(stop_thread);
			thread_started = false;
			if (thread->joinable())
			{
				thread->join();
			}
		}
		else if (thread_started && line == "%reset")
		{
			std::string reset_thread = "reset";
			inq->push(reset_thread);
			thread_started = false;
			if (thread->joinable())
			{
				thread->join();
			}
			Interpreter reset_interp = copy_interp;
			*interp = reset_interp;
			thread = new std::thread(thread_eval, interp, inq, outq);
			thread_started = true;
		}
		else if (thread_started && line == "%exit")
		{
			std::string exit_thread = "exit";
			inq->push(exit_thread);
			thread_started = false;
			if (thread->joinable())
			{
				thread->join();
			}
			//std::cout << "signal kill" << std::endl;
			//signal(SIGABRT, int_handler);
			//exit;
			//kill(getpid(), SIGKILL);
			return EXIT_SUCCESS;
		}
		else if (!thread_started)
		{
			std::cerr << "Error: thread not running" << std::endl;
		}
		else
		{
			inq->push(line);


			//outq->wait_and_pop(*outType);
			while (!outq->try_pop(*outType)) {
				if (global_status_flag > 0) {
					std::cout << "Error: interpreter kernel interrupted.";
					thread->detach();
					thread->~thread();
					inq = new InputQueue;
					outq = new OututQueue;
					outType = new outputq;
					//outType = new outputq;
					interp = new Interpreter;
					thread = new std::thread(thread_eval, interp, inq, outq);
					*interp = copy_interp;
					
					break;
				}
			}


			if (outType->type == 1)
			{
				std::cout << outType->exp << std::endl;
			}
			else if (outType->type == 0)
			{

				std::cout << outType->str << std::endl;
			}

		}
	}
	return EXIT_SUCCESS;
}

void thread_eval(Interpreter *interp, InputQueue *inq, OututQueue *outq)
{
	outputq outType;
	std::string line;
	while (1)
	{
		inq->wait_and_pop(line);
		if (line == "stop")
		{
			break;
		}
		else if (line == "reset")
		{
			//Environment * env = new Environment;
			//env.reset();
			break;
		}
		else if (line == "exit")
		{
			break;
		}
		else
		{
			std::istringstream expression(line);

			if (!interp->parseStream(expression)) {
				std::string str = "Invalid Expression. Could not parse";
				outType.str = str;
				outType.type = 0;
				outq->push(outType);
			}
			else {
				try {
					outType.exp = interp->evaluate();
					outType.type = 1;
					outq->push(outType);
				}
				catch (const SemanticError & ex) {
					std::string str = ex.what();
					outType.str = str;
					outType.type = 0;
					outq->push(outType);
				}
			}
		}
	}
	
}

/*liwunceigriuenhociq'mwfqhfpnovu;of;oiqfr
wrfoqirwf;qoirfq
ewqowrqrwijfq
rw

fwfriojqwrijfqwoi*/


int main(int argc, char *argv[])
{
	//eval_from_file(STARTUP_FILE);

	
	

	if (argc == 2) {
		return eval_from_file(argv[1]);
	}
	else if (argc == 3) {
		if (std::string(argv[1]) == "-e") {
			return eval_from_command(argv[2]);
		}
		else {
			error("Incorrect number of command line arguments.");
		}
	}
	else {
		eval_from_file(STARTUP_FILE);
		//repl();
	}

	return EXIT_SUCCESS;
}
