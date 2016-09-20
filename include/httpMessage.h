// httpRequest.h
#ifndef HTTPMESSAGE_H_
#define HTTPMESSAGE_H_

#include <string>
using std::string;

namespace http {

	struct Cache_Control {
		int max_age = 0;
	};

	enum Method {GET, POST, HEAD, DELETE, PUT, ERR};
	enum Accept_Encoding {gzip, deflate, sdch};
	enum status_code { OK=200, Bad_Request=400, Unauthorized = 401,
		Forbidden = 403, Not_Found = 404,
		Internal_Server_Error = 500, Server_Unavailable = 503 };

	class httpMsg {
	};


	class httpRequest {
		private:
			Method method;
			string url;
			string http_version;
			string Host;
			string Connection;
			Cache_Control cache_control;
			int UIR; // Upgrade-Insecure-Requests
			string user_agent;
			string accept;
			string referer;
			string acpt_encd;
			string acpt_lang;
			char data[1024];
			bool set_attr(const string &attr, const string& value);
			void reslove_req_line(string &line);
		
		public:
			httpRequest() {}
			httpRequest(char *http_str);
			bool from_str(char *http_str);
			void print();
			inline string getUrl() { return url; }
			inline char* getData() { return data; }
	};

	class httpResponse {
	private:
		string http_version;
		int code;
		string phrase;
		string content_type;
		size_t content_length;
		char *datapath;
	public:
		httpResponse();

		httpResponse(int code_, const string& phrase_,
				const string &type, size_t length);

		void set(int code_, const string& phrase_,
				const string &type, size_t length);

		void set_code(int code_);

		void set_phrase(const string &phrase_);

		void set_content_type(const string &type_);

		void set_content_length(size_t content_length_);
		
		void send_head(int fd);
	};
}; // end namespace


#endif
