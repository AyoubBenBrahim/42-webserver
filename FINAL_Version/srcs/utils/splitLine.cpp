# include <Utils.hpp>

std::vector<std::string>	splitLine(std::string line, std::string delimiter) {
	std::vector<std::string>	result;
	std::string					tmpLine;
	size_t						end;

	end = 0;
	while (line.size()) {
		end = line.find(delimiter);
		if (end == std::string::npos)
			end = line.length();
		if (end > 0) {
			tmpLine = line.substr(0, end);
			result.push_back(tmpLine);
		}
        if (end + delimiter.size() > line.size())
            break ;
        line = &line[end + delimiter.size()];
	}
	return (result);
} 