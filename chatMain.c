#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

int main() {
    // Set up the API request
    std::string api_key = "YOUR_API_KEY";
    std::string prompt = "Hello, world!";
    std::string url = "https://api.openai.com/v1/engines/davinci-codex/completions";
    std::string data = "{\"prompt\": \"" + prompt + "\", \"max_tokens\": 5}";

    // Set up the HTTP request
    CURL *curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        // Send the HTTP request and get the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t {
            ((std::string*)userdata)->append(ptr, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);

        // Parse the JSON response
        if (res == CURLE_OK) {
            Json::Value root;
            Json::CharReaderBuilder builder;
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            std::string errors;
            bool success = reader->parse(response.c_str(), response.c_str() + response.size(), &root, &errors);
            if (success) {
                std::cout << root["choices"][0]["text"].asString() << std::endl;
            } else {
                std::cerr << "Failed to parse JSON: " << errors << std::endl;
            }
        } else {
            std::cerr << "Failed to send HTTP request: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }

    return 0;
}
