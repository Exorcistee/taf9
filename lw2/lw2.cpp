#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <map>

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

void ReadMealy(std::vector<std::vector<std::string>>& tableIn, std::ifstream& inputFile)
{
	std::string line;
	int lineCount = 1;
	int xCount = 0;
	while (getline(inputFile, line))
	{
		if (lineCount != 1)
		{
			ParseLineMealy(tableIn, line);
			xCount++;
		}
		else
		{
			ParseLineMoore(tableIn, line);
			lineCount++;
		}
	}
}

void ReadMoore(std::vector<std::vector<std::string>>& tableIn, std::ifstream& inputFile)
{
	std::string line;
	while (getline(inputFile, line))
	{
		ParseLineMoore(tableIn, line);
	}
}

std::map<std::string, int> GetSignalsAndTheirCountForTheFirstTime(const std::set<std::string>& differentSignals, const std::vector<std::vector<std::string>>& tableIn)
{
	std::map<std::string, int> signalsAndTheirCount;

	auto it{ differentSignals.begin() };
	while (it != differentSignals.end())
	{
		int countOfSignal = 0;
		for (size_t j = 1; j < tableIn[0].size(); j++)
		{
			if (tableIn[0][j] == *it)
			{
				countOfSignal++;
			}
		}
		signalsAndTheirCount.insert(std::make_pair(*it, countOfSignal));
		it++;
	}

	return signalsAndTheirCount;
}

std::map<std::string, int> GetSignalsAndTheirCount(const std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>& newCountOfSignalsVector)
{
	int signalCount = 0;
	std::map<std::string, int> signalsAndTheirCount;
	for (const auto& el : newCountOfSignalsVector)
	{
		for (const auto& e : el)
		{
			signalCount++;
			signalsAndTheirCount.insert(std::make_pair(std::to_string(signalCount), int(e.second.size())));
		}
	}
	return signalsAndTheirCount;
}

void FillTableWithStartValues(std::vector<std::vector<std::string>>& tableTemp, const std::vector<std::vector<std::string>>& tableIn, const std::set<std::string>& differentSignals, std::map<std::string, int> signalsAndTheirCount)
{
	for (size_t i = 2; i < tableTemp.size(); i++)
	{
		tableTemp[i][0] = 'x' + std::to_string(i - 1);
	}

	int index = 1;
	for (auto el : signalsAndTheirCount)
	{
		for (int i = 0; i < el.second; i++)
		{
			tableTemp[0][index] = el.first;
			index++;
		}
	}
	index = 1;
	for (auto el : signalsAndTheirCount)
	{
		for (int i = 1; i < tableIn[0].size(); i++)
		{
			if (el.first == tableIn[0][i])
			{
				tableTemp[1][index] = tableIn[1][i];
				index++;
			}
		}
	}
	index = 1;
	std::string prevSignal = tableTemp[0][index];
	tableTemp[0][index] = "1";
	for (size_t i = 2; i < tableTemp[0].size(); i++)
	{
		std::string curSignal = tableTemp[0][i];
		if (curSignal != prevSignal)
		{
			prevSignal = tableTemp[0][i];
			index++;
		}
		tableTemp[0][i] = std::to_string(index);
	}
}

size_t FindIndexOfTable(std::string condition, const std::vector<std::vector<std::string>>& tableIn)
{
	for (size_t i = 1; i < tableIn[1].size(); i++)
	{
		if (condition == tableIn[1][i])
		{
			return i;
		}
	}
	return -1;
}

void FillTableWithAllValues(std::vector<std::vector<std::string>>& tableTemp, const std::vector<std::vector<std::string>>& tableIn)
{
	for (size_t i = 1; i < tableTemp[1].size(); i++)
	{
		size_t indexOfTableIn = FindIndexOfTable(tableTemp[1][i], tableIn);
		for (size_t j = 2; j < tableTemp.size(); j++)
		{
			size_t indexOfTableTemp = FindIndexOfTable(tableIn[j][indexOfTableIn], tableTemp);
			tableTemp[j][i] = tableTemp[0][indexOfTableTemp];
		}
	}
}

std::vector<std::string> GetTransitions(size_t i, const std::vector<std::vector<std::string>>& table)
{
	std::vector<std::string> result;
	for (size_t j = 2; j < table.size(); j++)
	{
		result.emplace_back(table[j][i]);
	}
	return result;
}

std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> GetNewSignals(const std::vector<std::vector<std::string>>& table, std::map<std::string, int> signalsAndTheirCount)
{
	size_t i = 1;
	std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> res;
	while (i < table[1].size())
	{
		for (auto el : signalsAndTheirCount)
		{
			std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> newSignalsVector;
			std::vector<std::string> transitions = GetTransitions(i, table);
			std::vector<std::string> signal{ table[1][i] };
			std::pair<std::vector<std::string>, std::vector<std::string>> transitionsAndTheirSignals;
			transitionsAndTheirSignals = std::make_pair(transitions, signal);
			for (int j = 1; j < el.second; j++)
			{
				std::vector<std::string> nextTransitions = GetTransitions(i + j, table);
				if (transitions != nextTransitions)
				{
					bool pushed = false;
					for (auto& e : newSignalsVector)
					{
						if (e.first == nextTransitions)
						{
							e.second.emplace_back(table[1][i + j]);
							pushed = true;
						}
					}
					if (!pushed)
					{
						newSignalsVector.push_back(transitionsAndTheirSignals);
						transitions = nextTransitions;
						signal.clear();
						signal = { table[1][i + j] };
						transitionsAndTheirSignals = std::make_pair(transitions, signal);
					}
				}
				else
				{
					transitionsAndTheirSignals.second.emplace_back(table[1][i + j]);
				}
			}
			i += el.second;
			newSignalsVector.emplace_back(transitionsAndTheirSignals);
			res.emplace_back(newSignalsVector);
		}
	}
	return res;
}

void FillTableWithStartValuesSecondLap(std::vector<std::vector<std::string>>& table, std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> newCountOfSignalsVector, std::map<std::string, int> signalsAndTheirCount)
{
	for (size_t i = 2; i < table.size(); i++)
	{
		table[i][0] = 'x' + std::to_string(i - 1);
	}
	int index = 1;
	for (auto el : signalsAndTheirCount)
	{
		for (int i = 0; i < el.second; i++)
		{
			table[0][index] = el.first;
			index++;
		}
	}
	index = 1;
	for (const auto& el : newCountOfSignalsVector)
	{
		for (const auto& e : el)
		{
			for (size_t i = 0; i < e.second.size(); i++)
			{
				table[1][index] = e.second[i];
				index++;
			}
		}
	}
}

size_t CalculateNewCountOfSignals(const std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>& newCountOfSignalsVector)
{
	size_t newCountOfSignals = 0;
	for (auto el : newCountOfSignalsVector)
	{
		newCountOfSignals += el.size();
	}
	return newCountOfSignals;
}

std::vector<std::vector<std::string>> MinimizeMoore(std::vector<std::vector<std::string>>& tableIn)
{
	std::set<std::string> differentSignals;
	for (size_t i = 1; i < tableIn[0].size(); i++)
	{
		differentSignals.insert(tableIn[0][i]);
	}
	std::vector<std::vector<std::string>> tableTemp(tableIn.size(), std::vector<std::string>(tableIn[0].size()));
	std::map<std::string, int> signalsAndTheirCount = GetSignalsAndTheirCountForTheFirstTime(differentSignals, tableIn);
	FillTableWithStartValues(tableTemp, tableIn, differentSignals, signalsAndTheirCount);
	FillTableWithAllValues(tableTemp, tableIn);
	std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> newCountOfSignalsVector = GetNewSignals(tableTemp, signalsAndTheirCount);

	size_t newCountOfSignals = CalculateNewCountOfSignals(newCountOfSignalsVector);

	if (newCountOfSignals == tableIn[0].size() - 1)
	{
		std::cout << "Automata has been minimized" << std::endl;
		return tableIn;
	}

	while (newCountOfSignals != differentSignals.size())
	{
		differentSignals.clear();
		for (size_t i = 1; i <= newCountOfSignals; i++)
		{
			differentSignals.insert(std::to_string(i));
		}
		signalsAndTheirCount.clear();
		signalsAndTheirCount = GetSignalsAndTheirCount(newCountOfSignalsVector);
		FillTableWithStartValuesSecondLap(tableTemp, newCountOfSignalsVector, signalsAndTheirCount);
		FillTableWithAllValues(tableTemp, tableIn);
		newCountOfSignalsVector = GetNewSignals(tableTemp, signalsAndTheirCount);
		newCountOfSignals = CalculateNewCountOfSignals(newCountOfSignalsVector);

		if (newCountOfSignals == differentSignals.size())
		{
			std::vector<std::vector<std::string>> formatedTable;
			formatedTable.resize(tableTemp.size());
			for (size_t i = 0; i < formatedTable.size(); i++)
			{
				formatedTable[i].resize(newCountOfSignals + 1);
			}
			for (size_t i = 1; i < formatedTable.size(); i++)
			{
				formatedTable[i][0] = tableTemp[i][0];
			}
			size_t indexFormatedtable = 1, indexTableTemp = 1;
			for (const auto& el : signalsAndTheirCount)
			{
				formatedTable[1][indexFormatedtable] = "q" + std::to_string(indexFormatedtable);
				for (size_t j = 2; j < formatedTable.size(); j++)
				{
					formatedTable[j][indexFormatedtable] = "q" + tableTemp[j][indexTableTemp];
				}
				indexTableTemp += el.second;
				indexFormatedtable++;
			}
			int index = 0;
			for (const auto& el : newCountOfSignalsVector)
			{
				for (const auto& e : el)
				{
					index++;
					for (size_t i = 1; i < tableIn[1].size(); i++)
					{
						if (e.second[0] == tableIn[1][i])
						{
							formatedTable[0][index] = tableIn[0][i];
							break;
						}
					}
				}
			}
			if (newCountOfSignals == tableIn[0].size() - 1)
			{
				std::cout << "Automata has been minimized" << std::endl;
			}
			return formatedTable;
		}
	}
	return tableIn;
}

std::vector<std::pair<std::string, int>> GetSignalsAndTheirCountMealey(std::vector<std::vector<std::pair<std::string, std::string>>> conditionsAndSignals)
{
	std::vector<std::pair<std::string, int>> signalsAndTheirCount;
	for (size_t i = 0; i < conditionsAndSignals[0].size(); i++)
	{
		std::string signals;
		for (size_t b = 0; b < conditionsAndSignals.size(); b++)
		{
			signals += conditionsAndSignals[b][i].second;
		}
		bool isFound = false;
		for (size_t j = 0; j < signalsAndTheirCount.size(); j++)
		{
			if (signalsAndTheirCount[j].first == signals)
			{
				signalsAndTheirCount[j].second++;
				isFound = true;
				break;
			}
		}
		if (!isFound)
		{
			signalsAndTheirCount.emplace_back(std::make_pair(signals, 1));
		}
	}
	return signalsAndTheirCount;
}

void FillTableWithStartValuesMealy(std::vector<std::vector<std::string>>& tableTemp, const std::vector<std::pair<std::string, int>>& signalsAndTheirCount, const std::vector<std::vector<std::string>>& tableIn, std::vector<std::vector<std::pair<std::string, std::string>>> conditionsAndSignals)
{
	for (size_t i = 2; i < tableTemp.size(); i++)
	{
		tableTemp[i][0] = tableIn[i - 1][0];
	}
	int signalIndex = 1;
	size_t index = 1;
	while (index < tableIn[0].size())
	{
		for (const auto& el : signalsAndTheirCount)
		{
			for (int i = 0; i < el.second; i++)
			{
				tableTemp[0][i + index] = std::to_string(signalIndex);
			}
			index += el.second;
			signalIndex++;
		}
	}
	int curIndex = 1;
	for (const auto& el : signalsAndTheirCount)
	{
		index = 1;
		while (index < tableIn[0].size())
		{
			std::string signal;
			for (size_t i = 0; i < conditionsAndSignals.size(); i++)
			{
				signal += conditionsAndSignals[i][index - 1].second;
			}
			if (signal == el.first)
			{
				tableTemp[1][curIndex] = tableIn[0][index];
				curIndex++;
			}
			index++;
		}
	}
}

size_t FindIndexOfTableMealy(std::string condition, const std::vector<std::vector<std::string>>& table)
{
	for (size_t i = 1; i < table[0].size(); i++)
	{
		if (condition == table[0][i])
		{
			return i;
		}
	}
	return -1;
}

void FillTableWithAllValuesMealy(std::vector<std::vector<std::string>>& tableTemp, const std::vector<std::vector<std::string>>& tableIn)
{
	for (size_t i = 1; i < tableTemp[1].size(); i++)
	{
		size_t indexOfTableIn = FindIndexOfTableMealy(tableTemp[1][i], tableIn);
		for (size_t j = 2; j < tableTemp.size(); j++)
		{
			size_t slashPos = tableIn[j - 1][indexOfTableIn].find('/');
			std::string condition = tableIn[j - 1][indexOfTableIn].substr(0, slashPos);
			size_t indexOfCondition = FindIndexOfTable(condition, tableTemp);
			tableTemp[j][i] = tableTemp[0][indexOfCondition];
		}
	}
}

std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> GetNewSignalsMealy(const std::vector<std::vector<std::string>>& table, std::vector<std::pair<std::string, int>> signalsAndTheirCount)
{
	size_t i = 1;
	std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> res;
	while (i < table[1].size())
	{
		for (auto el : signalsAndTheirCount)
		{
			std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> newSignalsVector;
			std::vector<std::string> transitions = GetTransitions(i, table);
			std::vector<std::string> signal{ table[1][i] };
			std::pair<std::vector<std::string>, std::vector<std::string>> transitionsAndTheirSignals;
			transitionsAndTheirSignals = std::make_pair(transitions, signal);
			for (int j = 1; j < el.second; j++)
			{
				std::vector<std::string> nextTransitions = GetTransitions(i + j, table);
				if (transitions != nextTransitions)
				{
					bool pushed = false;
					for (auto& e : newSignalsVector)
					{
						if (e.first == nextTransitions)
						{
							e.second.emplace_back(table[1][i + j]);
							pushed = true;
						}
					}
					if (!pushed)
					{
						newSignalsVector.push_back(transitionsAndTheirSignals);
						transitions = nextTransitions;
						signal.clear();
						signal = { table[1][i + j] };
						transitionsAndTheirSignals = std::make_pair(transitions, signal);
					}
				}
				else
				{
					transitionsAndTheirSignals.second.emplace_back(table[1][i + j]);
				}
			}
			i += el.second;
			newSignalsVector.emplace_back(transitionsAndTheirSignals);
			res.emplace_back(newSignalsVector);
		}
	}

	return res;
}

void FillTableWithStartValuesSecondLapMealy(std::vector<std::vector<std::string>>& table, std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> newCountOfSignalsVector, std::vector<std::pair<std::string, int>> signalsAndTheirCount)
{
	for (size_t i = 2; i < table.size(); i++)
	{
		table[i][0] = 'x' + std::to_string(i - 1);
	}
	int index = 1;
	for (auto el : signalsAndTheirCount)
	{
		for (int i = 0; i < el.second; i++)
		{
			table[0][index] = el.first;
			index++;
		}
	}
	index = 1;
	for (const auto& el : newCountOfSignalsVector)
	{
		for (const auto& e : el)
		{
			for (size_t i = 0; i < e.second.size(); i++)
			{
				table[1][index] = e.second[i];
				index++;
			}
		}
	}
}

std::vector<std::pair<std::string, int>> GetSignalsAndTheirCountMealy(const std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>>& newCountOfSignalsVector)
{
	int signalCount = 0;
	std::vector<std::pair<std::string, int>> signalsAndTheirCount;
	for (const auto& el : newCountOfSignalsVector)
	{
		for (const auto& e : el)
		{
			signalCount++;
			signalsAndTheirCount.emplace_back(std::make_pair(std::to_string(signalCount), int(e.second.size())));
		}
	}
	return signalsAndTheirCount;
}

std::vector<std::vector<std::string>> MinimizeMealy(std::vector<std::vector<std::string>>& tableIn)
{
	std::vector<std::vector<std::pair<std::string, std::string>>> conditionsAndSignals(tableIn.size() - 1, std::vector<std::pair<std::string, std::string>>(tableIn[1].size() - 1));
	for (size_t i = 1; i < tableIn.size(); i++)
	{
		for (size_t j = 1; j < tableIn[i].size(); j++)
		{
			std::string condition, signal;
			size_t slashPos = tableIn[i][j].find('/');
			condition = tableIn[i][j].substr(0, slashPos);
			signal = tableIn[i][j].substr(slashPos + 1, tableIn[i][j].size() - slashPos);
			conditionsAndSignals[i - 1][j - 1] = std::make_pair(condition, signal);
		}
	}
	std::vector<std::vector<std::string>> tableTemp(tableIn.size() + 1, std::vector<std::string>(tableIn[0].size()));
	std::vector<std::pair<std::string, int>> signalsAndTheirCount = GetSignalsAndTheirCountMealey(conditionsAndSignals);
	FillTableWithStartValuesMealy(tableTemp, signalsAndTheirCount, tableIn, conditionsAndSignals);
	FillTableWithAllValuesMealy(tableTemp, tableIn);
	std::vector<std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>>> newCountOfSignalsVector = GetNewSignalsMealy(tableTemp, signalsAndTheirCount);

	size_t newCountOfSignals = CalculateNewCountOfSignals(newCountOfSignalsVector);

	if (newCountOfSignals == tableIn[0].size() - 1)
	{
		std::cout << "Automata has been minimized" << std::endl;
		return tableIn;
	}

	while (newCountOfSignals != signalsAndTheirCount.size())
	{
		std::vector<std::vector<std::pair<std::string, std::string>>> conditionsAndSignals(tableIn.size() - 1, std::vector<std::pair<std::string, std::string>>(tableIn[1].size() - 1));
		for (size_t i = 1; i < tableIn.size(); i++)
		{
			for (size_t j = 1; j < tableIn[i].size(); j++)
			{
				std::string condition, signal;
				size_t slashPos = tableIn[i][j].find('/');
				condition = tableIn[i][j].substr(0, slashPos);
				signal = tableIn[i][j].substr(slashPos + 1, tableIn[i][j].size() - slashPos);
				conditionsAndSignals[i - 1][j - 1] = std::make_pair(condition, signal);
			}
		}
		signalsAndTheirCount.clear();
		signalsAndTheirCount = GetSignalsAndTheirCountMealy(newCountOfSignalsVector);
		FillTableWithStartValuesSecondLapMealy(tableTemp, newCountOfSignalsVector, signalsAndTheirCount);
		FillTableWithAllValuesMealy(tableTemp, tableIn);
		newCountOfSignalsVector = GetNewSignalsMealy(tableTemp, signalsAndTheirCount);
		newCountOfSignals = CalculateNewCountOfSignals(newCountOfSignalsVector);

		if (newCountOfSignals == signalsAndTheirCount.size())
		{
			std::vector<std::vector<std::string>> formatedTable;
			formatedTable.resize(tableIn.size());
			for (size_t i = 0; i < formatedTable.size(); i++)
			{
				formatedTable[i].resize(newCountOfSignals + 1);
			}
			for (size_t i = 2; i < formatedTable.size() + 1; i++)
			{
				formatedTable[i - 1][0] = tableTemp[i][0];
			}
			size_t indexFormatedtable = 1, indexTableTemp = 1;
			for (const auto& el : signalsAndTheirCount)
			{
				formatedTable[0][indexFormatedtable] = "q" + std::to_string(indexFormatedtable);
				for (size_t j = 1; j < formatedTable.size(); j++)
				{
					formatedTable[j][indexFormatedtable] = "q" + tableTemp[j + 1][indexTableTemp];
				}
				indexTableTemp += el.second;
				indexFormatedtable++;
			}
			int index = 0;
			for (const auto& el : newCountOfSignalsVector)
			{
				for (const auto& e : el)
				{
					index++;
					for (size_t i = 1; i < tableIn[1].size(); i++)
					{
						if (e.second[0] == tableIn[1][i])
						{
							formatedTable[0][index] = tableIn[0][i];
							break;
						}
					}
				}
			}
			index = 1;
			while (index < formatedTable[1].size())
			{
				for (const auto& el : newCountOfSignalsVector)
				{
					for (const auto& e : el)
					{
						size_t indexInTable = FindIndexOfTableMealy(e.second[0], tableIn);
						for (size_t k = 1; k < formatedTable.size(); k++)
						{
							size_t slashPos = tableIn[k][indexInTable].find('/');
							std::string signal = tableIn[k][indexInTable].substr(slashPos + 1, tableIn[k][indexInTable].size() - slashPos);
							formatedTable[k][index] += "/" + signal;
						}
					}
					index++;
				}
			}
			if (newCountOfSignals == tableIn[0].size() - 1)
			{
				std::cout << "Automata has been minimized" << std::endl;
			}
			return formatedTable;
		}
	}
	return tableIn;
}

void PrintTable(std::vector<std::vector<std::string>> table, std::ostream& output)
{
	for (size_t i = 0; i < table.size(); i++)
	{
		for (size_t j = 0; j < table[i].size(); j++)
		{
			output << table[i-1][j] << ';';
		}
		output << '\n';
	}
}
int main(int argc, char* argv[])
{
	std::string automatonType = argv[1],
		inputFileName = argv[2],
		outputFileName = argv[3];

	std::vector<std::vector<std::string>> tableIn, tableOut;
	std::ifstream inputFile(inputFileName);
	std::ofstream outputFile(outputFileName);

	if (automatonType == "mealy")
	{
		ReadMealy(tableIn, inputFile);
		tableOut = MinimizeMealy(tableIn);
		PrintTable(tableOut, outputFile);
	}
	else if (automatonType == "moore")
	{
		ReadMoore(tableIn, inputFile);
		tableOut = MinimizeMoore(tableIn);
		PrintTable(tableOut, outputFile);
	}
	else
	{
		std::cout << "Wrong automaton type" << std::endl;
	}
	return 0;
}
