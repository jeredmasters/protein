#pragma once
#include <string>
#include "parameters.h"

#include <iostream>
#include <string>
#include <curl/curl.h>

class communicator {
	private:
		std::string _data;
		std::string _base;
		std::string _token;
		size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
		std::string getToken();

	public:
		parameters * getParameters();
		communicator(std::string base);
		void pushResults(std::string testId, long fitness, long millis);
		void pushChromosome(std::string testId, std::string chromosome, long fitness, long millis);
};