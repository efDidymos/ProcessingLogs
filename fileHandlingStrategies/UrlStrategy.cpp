//
// Created by tomas on 28.3.2016.
//

#include "UrlStrategy.h"

#include <iostream>
#include <boost/asio.hpp>
#include <regex>

void UrlStrategy::execute()
{
    std::cout << "URL = " << url << std::endl;

//    std::regex e(R"delim(^((http[s]?|ftp):\/)?\/?([^:\/\s]+)((\/\w+)*\/)([\w\-\.]+[^#?\s]+)(.*)?(#[\w\-]+)?$)delim");
    std::regex e("^((http[s]?|ftp):\\/)?\\/?([^:\\/\\s]+)((\\/\\w+)*\\/)([\\w\\-\\.]+[^#?\\s]+)(.*)?(#[\\w\\-]+)?$");

    // Check if the argument is some type of URL
    if (std::regex_match(url, e))
    {
        std::cout << "Valid URL" << std::endl;
    }
    else
    {
        std::cout << "!NOT Valid URL" << std::endl;
    }
}