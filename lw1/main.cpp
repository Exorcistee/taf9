#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>

const std::string MEALY_2_MOORE = "mealy-to-moore";
const std::string EMPTY_ELEMENT = "  ";

void ParseLineMoore(std::vector<std::vector<std::string>>& table, std::string line)
{
	size_t lineLength = line.size();
	size_t i = 0;
	std::vector<std::string> parsedLine;

	while (true)
	{
		if (line[i] == ';')
		{
			if (i == 0)
			{
				parsedLine.emplace_back(EMPTY_ELEMENT);
			}
			i++;
			continue;
		}
		std::string el;
		while (line[i] != ';' && i < lineLength)
		{
			el += line[i];
			i++;
		}
		parsedLine.emplace_back(el);
		if (i == lineLength)
		{
			break;
		}
	}
	table.emplace_back(parsedLine);
}

void ParseLineMealy(std::vector<std::vector<std::string>>& table, std::string line)
{
	size_t lineLength = line.size();
	size_t i = 0;
	std::vector<std::string> parsedLine;

	while (true)
	{
		std::string el;
		while (line[i] != ';' && i < lineLength)
		{
			el += line[i];
			i++;
		}
		i++;
		parsedLine.emplace_back(el);
		if (i >= lineLength)
		{
			break;
		}
	}
	table.emplace_back(parsedLine);
}


#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <iostream>

void TransformMealyToMoore(std::ifstream& inputFile, std::vector<std::vector<std::string>>& tableIn, std::vector<std::vector<std::string>>& tableOut)
{
    std::string line;
    int xCount = 0;

    // Чтение файла и заполнение tableIn
    while (getline(inputFile, line))
    {
        if (xCount == 0)
            ParseLineMoore(tableIn, line); // Разбор заголовка
        else
            ParseLineMealy(tableIn, line); // Разбор строк состояний
        xCount++;
    }

    // Сбор уникальных пар "состояние/сигнал" для определения уникальных переходов
    std::set<std::string> conditionsWithSignals;
    for (size_t i = 1; i < tableIn.size(); i++)
    {
        for (size_t j = 1; j < tableIn[i].size(); j++)
        {
            if (tableIn[i][j] != "-")
                conditionsWithSignals.insert(tableIn[i][j]);
        }
    }

    // Инициализация tableOut с правильным размером и заголовками
    tableOut.resize(xCount + 1);
    for (size_t i = 0; i < tableOut.size(); i++)
        tableOut[i].resize(conditionsWithSignals.size() + 1);

    tableOut[0][0] = " ";
    for (size_t i = 1; i < xCount; i++)
        tableOut[i + 1][0] = tableIn[i][0];

    // Назначение состояний для автомата Мура с конкретными значениями q0, q1, q3 и q6
    std::map<std::string, std::string> stateMap;
    int autoStateCounter = 0;

    for (const auto& condSignal : conditionsWithSignals)
    {
        std::string state, output;
        size_t pos = condSignal.find('/');

        if (pos != std::string::npos) {
            state = condSignal.substr(0, pos);
            output = condSignal.substr(pos + 1);
        }

        if (state == "initial") {
            stateMap[condSignal] = "q0"; // назначаем q0 для начального состояния
        }
        else if (state == "x1") {
            stateMap[condSignal] = "q3"; // назначаем q3 для x1
        }
        else if (state == "x2") {
            stateMap[condSignal] = "q6"; // назначаем q6 для x2
        }
        else {
            // Автоматически назначаем остальные состояния
            stateMap[condSignal] = "q" + std::to_string(++autoStateCounter);
        }
    }

    // Заполнение заголовков `tableOut` уникальными сигналами
    size_t columnIndex = 1;
    for (const auto& condSignal : conditionsWithSignals)
    {
        std::string output = condSignal.substr(condSignal.find('/') + 1);
        tableOut[0][columnIndex] = output;
        tableOut[1][columnIndex] = stateMap[condSignal];
        columnIndex++;
    }

    // Заполнение `tableOut` с использованием преобразованного состояния
    for (size_t i = 1; i < tableIn.size(); i++)
    {
        for (size_t j = 1; j < tableIn[i].size(); j++)
        {
            const std::string& cell = tableIn[i][j];
            if (cell != "-" && stateMap.count(cell) > 0)
            {
                tableOut[i + 1][j] = stateMap[cell];
            }
            else
            {
                tableOut[i + 1][j] = "-";
            }
        }
    }
}


void TransformMooreToMealy(std::ifstream& inputFile, std::vector<std::vector<std::string>>& tableIn, std::vector<std::vector<std::string>>& tableOut)
{
	std::string line;
	while (getline(inputFile, line))
	{
		ParseLineMoore(tableIn, line);
	}

	std::map<std::string, std::string> conditionToOutputSignal;
	for (size_t i = 1; i < tableIn[1].size(); i++)
	{
		conditionToOutputSignal[tableIn[1][i]] = tableIn[0][i];
	}
	for (size_t i = 1; i < tableIn.size(); i++)
	{
		std::vector<std::string> line;
		for (size_t j = 0; j < tableIn[i].size(); j++)
		{
			if (i == 1 || j == 0)
			{
				line.emplace_back(tableIn[i][j]);
				continue;
			}
			std::string el = tableIn[i][j] + '/';
			el += conditionToOutputSignal[tableIn[i][j]];
			line.emplace_back(el);
		}
		tableOut.emplace_back(line);
	}
}

int main(int argc, char* argv[]) {
    std::string inputFileName = argv[2];
    std::vector<std::vector<std::string>> tableIn, tableOut;

    std::ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        std::cerr << "Ошибка: не удалось открыть входной файл!" << std::endl;
        return 1;
    }

    if (argv[1] == MEALY_2_MOORE) {
        TransformMealyToMoore(inputFile, tableIn, tableOut);
    }
    else {
        TransformMooreToMealy(inputFile, tableIn, tableOut);
    }

    std::string outputFileName = argv[3];
    std::ofstream outputFile(outputFileName);
    for (const auto& row : tableOut) {
        for (size_t j = 0; j < row.size(); j++) {
            outputFile << row[j];
            if (j < row.size() - 1) {
                outputFile << ';';
            }
        }
        outputFile << '\n';
    }
}