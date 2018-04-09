#include "stdafx.h"
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

communicator::communicator(string base) :
	_base(base) {
	_token = getToken();
}
std::string communicator::getToken() {
	string token;
	ifstream ifile("token.txt");
	if (ifile.good() && ifile.is_open() && getline(ifile, token)) {
		cout << token << '\n';
	}
	else {
		char buf[20];
		gen_random(buf, 20);
		token.assign(buf, 20);

		ofstream datfile;
		datfile.open("token.txt");
		datfile << token;
		datfile.close();
	}
	return token;
}

parameters * communicator::getParameters() {
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	curl = curl_easy_init();
	if (curl) {
		std::string url = _base + "new/";
		std::string xtoken("x-token: ");
		xtoken.append(_token);

		struct curl_slist *list = NULL;
		list = curl_slist_append(list, xtoken.c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::cout << readBuffer << std::endl;
	}
	parameters * params = new parameters();
	params->complete = true;
	if (readBuffer == "\"benchmark\"") {
		params->benchmark = true;
		params->id = "benchmark";
	}
	else {
		params->benchmark = false;
		vector<string> strings;
		istringstream f(readBuffer);
		string s;
		while (getline(f, s, ',')) {
			cout << s << endl;
			strings.push_back(s);
		}

		if (strings.size() < 9) {
			params->complete = false;
			return params;
		}
		params->id = strings[0];
		params->population = std::stoi(strings[1]);
		params->generations = std::stoi(strings[2]);
		params->selection_pressure = std::stoi(strings[3]);
		params->duration = std::stoi(strings[4]);
		params->crossover_rate = std::stoi(strings[5]);
		params->mutation_rate = std::stoi(strings[6]);
		params->mutation_variance = std::stoi(strings[7]);
		params->duration_variance = std::stoi(strings[8]);
	}
	

	return params;
}

void communicator::pushResults(std::string testId, long fitness, long millis) {
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	curl = curl_easy_init();
	if (curl) {
		std::string url = _base + "save/";
		std::string xtoken("x-token: ");
		xtoken.append(_token);
		
		char sfitness[20], smillis[20];
		_ltoa_s(fitness, sfitness, 10);
		_ltoa_s(millis, smillis, 10);

		std::string body("test=");
		body.append(testId);
		body.append("&fitness=");
		body.append(sfitness);
		body.append("&millis=");
		body.append(smillis);

		struct curl_slist *list = NULL;
		list = curl_slist_append(list, xtoken.c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::cout << readBuffer << std::endl;
	}
}
