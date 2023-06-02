#include "inkwell.h"

using namespace inkwell;

void Rule::setTriggeredBy(std::vector<int> triggeredBy, modOp operation)
{
	if (operation == SET)
		this->triggeredBy = triggeredBy;
	else if (operation == ADD)
		this->triggeredBy.insert(this->triggeredBy.end(), triggeredBy.begin(), triggeredBy.end());
	else
		std::cout << "error at Rule::setTriggeredBy\n";
}

void Rule::setTriggers(std::vector<int> triggers, modOp operation)
{
	if (operation == SET)
		this->triggers = triggers;
	else if (operation == ADD)
		this->triggers.insert(this->triggers.end(), triggers.begin(), triggers.end());
	else
		std::cout << "error at Rule::setTriggers\n";
}

void Rule::setCriteria(std::vector<Criterion> criteria, modOp operation)
{
	if (operation == SET)
		this->criteria = criteria;
	else if (operation == ADD)
		this->criteria.insert(this->criteria.end(), criteria.begin(), criteria.end());
	else
		std::cout << "error at Rule::setCriteria\n";
}

void Rule::setModifications(std::vector<Modification> modifications, modOp operation)
{
	if (operation == SET)
		this->modifications = modifications;
	else if (operation == ADD)
		this->modifications.insert(this->modifications.end(), modifications.begin(), modifications.end());
	else
		std::cout << "error at Rule::setModifications\n";
}

void Table::setEvents(std::vector<Event> events, modOp operation)
{
	if (operation == SET)
		this->events.clear();

	for (Event i : events)
		this->events.insert({ i.id, i });

	if (operation != SET and operation != ADD)
		std::cout << "error at Table::setEvents\n";
}

void Table::setFacts(std::vector<Fact> facts, modOp operation)
{
	if (operation == SET)
		this->facts.clear();

	for (Fact i : facts)
		this->facts.insert({ i.id, i });

	if (operation != SET and operation != ADD)
		std::cout << "error at Table::setFacts\n";
}

void Table::setRules(std::vector<Rule> rules, modOp operation)
{
	if (operation == SET)
		this->rules.clear();

	for (Rule i : rules)
		this->rules.insert({ i.id, i });

	if (operation != SET and operation != ADD)
		std::cout << "error at Table::setRules\n";
}

JSONParser::JSONParser(const std::string filename)
{
	this->fileInput.open(filename);
}

std::string JSONParser::getNextString()
{
	this->read = 0;
	std::string str = "";

	while (this->read != '"') // reach 1st quotation mark
		this->fileInput.get(this->read);
	this->fileInput.get(this->read); // skip 1st quotation mark lol

	while (this->read != '"')
	{
		str += this->read;
		this->fileInput.get(this->read);
	}
	// shove chars in str until 2nd quotation mark

	return str;
}

int JSONParser::getNextInteger()
{
	int integer = 0;
	bool isNegative = 0;
	std::vector<int> digits;

	while (this->read != '-' and !isdigit(this->read))
		this->fileInput.get(this->read);

	if (this->read == '-')
	{
		isNegative = 1;
		this->fileInput.get(this->read); // skip the minus
	}

	while (isdigit(this->read))
	{
		digits.push_back((int)this->read - 48);
		this->fileInput.get(this->read);
	}

	int p = 1;
	for (int i = (int)digits.size() - 1; i >= 0; --i)
	{
		integer += digits[i] * p;
		p *= 10;
	}

	if (isNegative)
		integer *= -1;

	return integer;
}

comparisonOp JSONParser::getNextComparisonOp()
{
	std::string op = this->getNextString();

	if(op == "equal")
		return EQUAL;
	else if(op == "notEqual")
		return NOT_EQUAL;
	else if(op == "lessThan")
		return LESS_THAN;
	else if(op == "lessThanOrEqual")
		return LESS_THAN_OR_EQUAL;
	else if(op == "greaterThan")
		return GREATER_THAN;
	else if(op == "greaterThanOrEqual")
		return GREATER_THAN_OR_EQUAL;
	
	return NULL_COMPARISONOP;
}

modOp JSONParser::getNextModOp()
{
	std::string op = this->getNextString();

	if (op == "SET")
		return SET;
	else if (op == "ADD")
		return ADD;
	else if (op == "INCREMENT")
		return INCREMENT;

	return NULL_MODOP;
}

Keys JSONParser::getNextKey()
{
	this->read = 0;

	this->fileInput.get(this->read); // skip 1st quotation mark
	std::string key = "";
	while (this->read != '"') // this->read until 2nd quotation mark
	{
		key += this->read;
		this->fileInput.get(this->read);
	}

	if (key == "name")
		return NAME;
	else if (key == "description")
		return DESCRIPTION;
	else if (key == "createdAt")
		return CREATEDAT;
	else if (key == "id")
		return ID;
	else if (key == "key")
		return KEY;
	else if (key == "tables")
		return TABLES;
	else if (key == "events")
		return EVENTS;
	else if (key == "facts")
		return FACTS;
	else if (key == "rules")
		return RULES;
	else if (key == "data")
		return DATA;
	else if (key == "triggeredBy")
		return TRIGGERED_BY;
	else if (key == "triggers")
		return TRIGGERS;
	else if (key == "criteria")
		return CRITERIA;
	else if (key == "modifications")
		return MODIFICATIONS;
	else if (key == "comparedEntry")
		return COMPARED_ENTRY;
	else if (key == "compareValue")
		return COMPARE_VALUE;
	else if (key == "comparisonOperator")
		return COMPARISON_OPERATOR;
	else if (key == "entry")
		return MODIFIED_ENTRY;
	else if (key == "modificationOperator")
		return MOD_OPERATOR;
	else if(key == "value")
		return VALUE;

	return NULL_KEYS;
}

std::vector<int> JSONParser::parseIntArray()
{
	this->read = 0;
	std::vector<int> arr;

	while (this->read != '[')
		this->fileInput.get(this->read);

	while (this->read != ']')
	{
		this->fileInput.get(this->read);

		if (this->read == '-' or isdigit(this->read))
			arr.push_back(this->getNextInteger());
	}

	return arr;
}

std::vector<Criterion> JSONParser::parseCriteria()
{
	this->read = 0;
	std::vector<Criterion> criteria;
	int nestingIndex = 1;

	while (this->read != '[')
		this->fileInput.get(this->read);

	while (nestingIndex > 0)
	{
		this->fileInput.get(this->read);

		if (this->read == '{' or this->read == '[')
		{
			++nestingIndex;
			continue;
		}
		else if (this->read == '}' or this->read == ']')
		{
			--nestingIndex;
			continue;
		}
		else if (this->read != '"')
			continue;

		Keys keyToken = this->getNextKey();

		switch (keyToken)
		{
		case COMPARED_ENTRY:
		{
			criteria.push_back(Criterion());
			criteria.back().comparedEntry = this->getNextInteger();
			break;
		}
		case COMPARE_VALUE:
		{
			criteria.back().compareValue = this->getNextInteger();
			break;
		}
		case COMPARISON_OPERATOR:
		{
			criteria.back().comparisonOperator = this->getNextComparisonOp();
			break;
		}
		default: std::cout << "Key seems to be invalid!\n";
		}
	}

	return criteria;
}

std::vector<Modification> JSONParser::parseModifications()
{
	this->read = 0;
	std::vector<Modification> modifications;
	int nestingIndex = 1;

	while (this->read != '[')
		this->fileInput.get(this->read);

	while (nestingIndex > 0)
	{
		this->fileInput.get(this->read);

		if (this->read == '{' or this->read == '[')
		{
			++nestingIndex;
			continue;
		}
		else if (this->read == '}' or this->read == ']')
		{
			--nestingIndex;
			continue;
		}
		else if (this->read != '"')
			continue;

		Keys keyToken = this->getNextKey();

		switch (keyToken)
		{
		case MODIFIED_ENTRY:
		{
			modifications.push_back(Modification());
			modifications.back().modifiedEntry = this->getNextInteger();
			break;
		}
		case MOD_OPERATOR:
		{
			modifications.back().modOperator = this->getNextModOp();
			break;
		}
		case VALUE:
		{
			modifications.back().value = this->getNextInteger();
			break;
		}
		default: std::cout << "Key seems to be invalid!\n";
		}
	}

	return modifications;
}

std::unordered_map<int, Event> JSONParser::parseEvents()
{
	this->read = 0;
	std::unordered_map<int, Event> events;
	int currentEventID = 0;
	int nestingIndex = 1;

	while (this->read != '[')
	{
		this->fileInput.get(this->read);
	}

	while (nestingIndex > 0) // events don't have any further nesting atm but this is more expandable
	{
		this->fileInput.get(this->read);

		if (this->read == '{' or this->read == '[')
		{
			++nestingIndex;
			continue;
		}
		else if (this->read == '}' or this->read == ']')
		{
			--nestingIndex;
			continue;
		}
		else if(this->read != '"')
			continue;

		Keys keyToken = this->getNextKey();

		switch (keyToken)
		{
		case ID: // start of new event
		{
			currentEventID = this->getNextInteger();
			events.insert({ currentEventID, Event() });
			events[currentEventID].id = currentEventID;
			break;
		}
		case KEY:
		{
			events[currentEventID].key = this->getNextString();
			break;
		}
		default: std::cout << "Key seems to be invalid!\n";
		}
	}

	return events;
}

std::unordered_map<int, Fact> JSONParser::parseFacts()
{
	this->read = 0;
	std::unordered_map<int, Fact> facts;
	int currentFactID = 0;
	int nestingIndex = 1;

	while (this->read != '[')
	{
		this->fileInput.get(this->read);
	}

	while (nestingIndex > 0)
	{
		this->fileInput.get(this->read);

		if (this->read == '{' or this->read == '[')
		{
			++nestingIndex;
			continue;
		}
		else if (this->read == '}' or this->read == ']')
		{
			--nestingIndex;
			continue;
		}
		else if (this->read != '"')
			continue;

		Keys keyToken = this->getNextKey();

		switch (keyToken)
		{
		case ID: // start of new fact
		{
			currentFactID = this->getNextInteger();
			facts.insert({ currentFactID, Fact() });
			facts[currentFactID].id = currentFactID;
			break;
		}
		case KEY:
		{
			facts[currentFactID].key = this->getNextString();
			break;
		}
		case DATA:
		{
			facts[currentFactID].data = this->getNextInteger();
			break;
		}
		default: std::cout << "Key seems to be invalid!\n";
		}
	}

	return facts;
}

std::unordered_map<int, Rule> JSONParser::parseRules()
{
	this->read = 0;
	std::unordered_map<int, Rule> rules;
	int currentRuleID = 0;
	int nestingIndex = 1;

	while (this->read != '[')
	{
		this->fileInput.get(this->read);
	}

	while (nestingIndex > 0)
	{
		this->fileInput.get(this->read);

		if (this->read == '{' or this->read == '[')
		{
			++nestingIndex;
			continue;
		}
		else if (this->read == '}' or this->read == ']')
		{
			--nestingIndex;
			continue;
		}
		else if (this->read != '"')
			continue;

		Keys keyToken = this->getNextKey();

		switch (keyToken)
		{
		case ID: // start of new rule
		{
			currentRuleID = this->getNextInteger();
			rules.insert({ currentRuleID, Rule() });
			rules[currentRuleID].id = currentRuleID;
			break;
		}
		case KEY:
		{
			rules[currentRuleID].key = this->getNextString();
			break;
		}
		case TRIGGERED_BY:
		{
			rules[currentRuleID].triggeredBy = this->parseIntArray();
			break;
		}
		case TRIGGERS:
		{
			rules[currentRuleID].triggers = this->parseIntArray();
			break;
		}
		case CRITERIA: 
		{
			rules[currentRuleID].criteria = this->parseCriteria();
			break;
		}
		case MODIFICATIONS:
		{
			rules[currentRuleID].modifications = this->parseModifications();
			break;
		}
		default: std::cout << "Key seems to be invalid!\n";
		}
	}

	return rules;
}

std::unordered_map<int, Table> JSONParser::parseTables()
{
	this->read = 0;
	std::unordered_map<int, Table> tables;
	int currentTableID = 0;
	int nestingIndex = 1;

	while (this->read != '[')
	{
		this->fileInput.get(this->read);
	}

	while (nestingIndex > 0)
	{
		this->fileInput.get(this->read);

		if (this->read == '{' or this->read == '[') // start of a table's component or an array within
		{
			++nestingIndex;
			continue;
		}
		else if (this->read == '}' or this->read == ']') // end of a table's component or an array within
		{
			--nestingIndex;
			continue;
		}
		else if (this->read != '"') // not a key
			continue;

		Keys keyToken = this->getNextKey();

		switch (keyToken)
		{
		case ID: // start of a new table
		{
			currentTableID = this->getNextInteger();
			tables.insert({ currentTableID, Table() });
			tables[currentTableID].id = currentTableID;
			break;
		}
		case KEY:
		{
			tables[currentTableID].key = this->getNextString();
			break;
		}
		case EVENTS:
		{
			tables[currentTableID].events = this->parseEvents();
			break;
		}
		case FACTS:
		{
			tables[currentTableID].facts = this->parseFacts();
			break;
		}
		case RULES:
		{
			tables[currentTableID].rules = this->parseRules();
			break;
		}
		default: std::cout << "Key seems to be invalid!\n";
		}
	}

	return tables;
} // placeholder function

Project JSONParser::parseProject()
{
	
	Project project;

	while (this->fileInput.get(this->read))
	{
		if (this->read != '"')
			continue;

		Keys keyToken = this->getNextKey();

		switch (keyToken)
		{
		case NAME:
		{
			project.name = this->getNextString(); 
			break;
		}
		case DESCRIPTION:
		{
			project.description = this->getNextString(); 
			break;
		}
		case CREATEDAT:
		{
			project.createdAtNano = this->getNextInteger(); 
			break;
		}
		case TABLES: 
		{
			project.tables = this->parseTables(); 
			break;
		}
		default: std::cout << "Key seems to be invalid!\n";
		}
	}

	return project;
}