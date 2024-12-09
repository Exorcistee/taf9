#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>

using namespace std;

using Table = vector<vector<string>>;

void PrintTable(const Table& table, ostream& output)
{
	for (const auto& rows : table)
	{
		for (const auto& columns : rows)
		{
			output << columns << ';';
		}
		output << endl;
	}
}

Table ReadTable(ifstream& input)
{
	Table NFA;
	string el;
	size_t strCount = 0;
	while (input >> el)
	{
		NFA.emplace_back(vector<string>());
		size_t i = 0;
		while (i < el.size())
		{
			string str;
			while (el[i] != ';' && i < el.size())
			{
				str += el[i];
				i++;
			}
			i++;
			NFA[strCount].emplace_back(str);
		}
		strCount++;
	}

	for (size_t i = 1; i < NFA.size(); i++)
	{
		if (NFA[i].size() < NFA[0].size())
		{
			NFA[i].resize(NFA[0].size());
		}
	}
	return NFA;
}

size_t FindSignalIndex(const std::string& signal, const Table& NFA)
{
	for (size_t i = 2; i < NFA.size(); i++)
	{
		if (NFA[i][0] == signal)
		{
			return i;
		}
	}
	throw;
}

vector<string> VertexHandle(const std::string& str)
{
	vector<string> result;
	size_t i = 0;
	while (i < str.size())
	{
		string vertex;
		while (str[i] != ',' && i < str.size())
		{
			vertex += str[i];
			i++;
		}
		result.emplace_back(vertex);
		i++;
	}
	return result;
}

size_t FindVertexIndex(const string& vertex, const Table& NFA)
{
	for (size_t i = 1; i < NFA[1].size(); i++)
	{
		if (NFA[1][i] == vertex)
		{
			return i;
		}
	}
	throw;
}

size_t FindSignalIndexInDFA(const string& signal, const Table& DFA)
{
	for (size_t i = 1; i < DFA[0].size(); i++)
	{
		if (DFA[0][i] == signal)
		{
			return i;
		}
	}
	throw;
}

bool CheckAttandenceTable(const string& vertex, const Table& DFA)
{
	for (size_t i = 1; i < DFA.size(); i++)
	{
		if (DFA[i][0] == vertex)
		{
			return true;
		}
	}
	return false;
}

bool CheckAttandenceVector(const string& vertex, const vector<string> vector)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == vertex)
		{
			return true;
		}
	}
	return false;
}

void InsertFormatedValue(const string& value, const map<string, vector<string>>& neededCalls, const map<string, vector<string>>& vertexAndEqualVertexes, Table& formatedDFA, size_t i, size_t j)
{
	string formatedValue;
	vector<string> vectorOfVertexFormatingValue;
	for (auto it = vertexAndEqualVertexes.begin(); it != vertexAndEqualVertexes.end(); it++)
	{
		if (it->first == value)
		{
			vectorOfVertexFormatingValue = it->second;
			break;
		}
		if (!vectorOfVertexFormatingValue.empty())
		{
			break;
		}
	}
	for (auto it = neededCalls.begin(); it != neededCalls.end(); it++)
	{
		if (it->second == vectorOfVertexFormatingValue)
		{
			formatedValue = it->first;
			break;
		}
	}
	if (formatedValue.empty())
	{
		for (auto it = neededCalls.begin(); it != neededCalls.end(); it++)
		{
			for (size_t i = 0; i < it->second.size(); i++)
			{
				if (it->second[i] == value)
				{
					formatedValue = it->first;
					break;
				}
			}
			if (!formatedValue.empty())
			{
				break;
			}
		}
	}
	formatedDFA[i + 1][j] = formatedValue;
}

size_t GetVertexPositionInVector(const string& vertex, const vector<string>& vector)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == vertex)
		{
			return i;
		}
	}
	throw;
}

Table TransformNFAToDFA(const Table& NFA)
{
	Table DFA;
	map<string, vector<string>> vertexAndEqualVertexes;
	for (size_t i = 1; i < NFA[1].size(); i++)
	{
		size_t j = FindSignalIndex("e", NFA);
		vector<string> transitVertexes;
		if (!NFA[j][i].empty())
		{
			transitVertexes = VertexHandle(NFA[j][i]);
		}
		transitVertexes.emplace_back(NFA[1][i]);
		vertexAndEqualVertexes.insert(make_pair(NFA[1][i], transitVertexes));
	}

	DFA.resize(1);
	DFA[0].resize(NFA.size() - 2);
	for (size_t i = 2; i < NFA.size() - 1; i++)
	{
		DFA[0][i - 1] = NFA[i][0];
	}
	vector<string> queue{ NFA[1][1] };
	while (!queue.empty())
	{
		std::string currVertex = queue[0];
		queue.erase(queue.begin());
		DFA.resize(DFA.size() + 1);
		DFA[DFA.size() - 1].emplace_back(currVertex);
		DFA[DFA.size() - 1].resize(DFA[0].size());
		vector<string> checkingVertexes = vertexAndEqualVertexes.find(currVertex)->second;
		for (size_t i = 0; i < checkingVertexes.size(); i++)
		{
			size_t columnPos = FindVertexIndex(checkingVertexes[i], NFA);
			for (size_t j = 2; j < NFA.size() - 1; j++)
			{
				if (!NFA[j][columnPos].empty())
				{
					size_t puttingPosDFA = FindSignalIndexInDFA(NFA[j][0], DFA);
					if (!DFA[DFA.size() - 1][puttingPosDFA].empty())
					{
						if (GetVertexPositionInVector(NFA[j][columnPos], checkingVertexes) < i)
						{
							continue;
						}
					}
					DFA[DFA.size() - 1][puttingPosDFA] += NFA[j][columnPos];
					if (!CheckAttandenceTable(NFA[j][columnPos], DFA) && !CheckAttandenceVector(NFA[j][columnPos], queue))
					{
						queue.emplace_back(NFA[j][columnPos]);
					}
				}
			}
		}
	}
	for (size_t i = 1; i < DFA.size(); i++)
	{
		bool isUnnecessary = true;
		for (size_t j = 1; j < DFA[i].size(); j++)
		{
			if (!DFA[i][j].empty())
			{
				isUnnecessary = false;
			}
		}
		if (isUnnecessary)
		{
			DFA.erase(DFA.begin() + i);
		}
	}
	Table formatedDFA;
	formatedDFA.resize(DFA[0].size() + 1, vector<string>(DFA.size()));
	for (size_t i = 1; i < DFA[0].size(); i++)
	{
		formatedDFA[i + 1][0] = DFA[0][i];
	}
	map<string, vector<string>> neededCalls;
	size_t index = 0;
	for (size_t i = 1; i < DFA.size(); i++)
	{
		neededCalls.insert(make_pair("s" + to_string(index), vertexAndEqualVertexes.find(DFA[i][0])->second));
		index++;
	}
	formatedDFA[1].resize(1);
	for (auto it = neededCalls.begin(); it != neededCalls.end(); it++)
	{
		formatedDFA[1].emplace_back(it->first);
	}

	for (size_t i = 1; i < DFA[0].size(); i++)
	{
		for (size_t j = 1; j < DFA.size(); j++)
		{
			if (DFA[j][i] != "-")
			{
				InsertFormatedValue(DFA[j][i], neededCalls, vertexAndEqualVertexes, formatedDFA, i, j);
			}
		}
	}

	for (size_t i = 2; i < formatedDFA.size(); i++)
	{
		for (size_t j = 1; j < formatedDFA[i].size(); j++)
		{
			if (formatedDFA[i][j].empty())
			{
				formatedDFA[i][j] = "-";
			}
		}
	}

	for (auto it = neededCalls.begin(); it != neededCalls.end(); it++)
	{
		cout << it->first << " = ";
		for (size_t i = 0; i < it->second.size(); i++)
		{
			cout << it->second[i] << ' ';
		}
		cout << endl;
	}

	string finishVertex;
	for (size_t i = 0; i < NFA[0].size(); i++)
	{
		if (NFA[0][i] == "F")
		{
			finishVertex = NFA[1][i];
		}
	}

	vector<string> finishVertexes;
	for (auto it = neededCalls.begin(); it != neededCalls.end(); it++)
	{
		for (size_t i = 0; i < it->second.size(); i++)
		{
			if (it->second[i] == finishVertex)
			{
				finishVertexes.emplace_back(it->first);
				break;
			}
		}
	}

	for (size_t j = 0; j < finishVertexes.size(); j++)
	{
		for (size_t i = 1; i < formatedDFA[1].size(); i++)
		{
			if (formatedDFA[1][i] == finishVertexes[j])
			{
				formatedDFA[0][i] = "F";
				break;
			}
		}
	}

	return formatedDFA;
}

int main(int argc, char* argv[])
{
	string inputFileName, outputFileName;
	inputFileName = argv[1];
	outputFileName = argv[2];
	ifstream input(inputFileName);
	ofstream output(outputFileName);
	Table NFA = ReadTable(input);
	Table DFA = TransformNFAToDFA(NFA);
	PrintTable(DFA, output);
	return 0;
}