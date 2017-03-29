#ifndef _CVAR_COM_H_
#define _CVAR_COM_H_

#include "j1Module.h"


enum ARGUMENTS_TYPE
{
	NONE,
	INT_VAR,
	CHAR_VAR,
	BOOL_VAR
};


//---------------COMAND-------------------
//----------------------------------------


struct command
{
	std::string name;
	unsigned int min_arguments;
	unsigned int max_arguments;

	j1Module* my_module;
	ARGUMENTS_TYPE args_type;

	command(const char*, j1Module*, unsigned int, unsigned int, ARGUMENTS_TYPE);
};

//----------------CVAR--------------------
//----------------------------------------

class CVar
{
	std::string name;
	std::string description;

	std::string value;

	int min_value, max_value;
	

	bool read_only;

public:

	CVar(const char* , const char* , const char*, int , int , j1Module*, ARGUMENTS_TYPE, bool);

	const char* Get_name() const
	{
		return name.c_str();
	}

	const char* Get_Description() const
	{
		return description.c_str();
	}

	int Get_value_Int() const
	{
		return atoi(value.c_str());
	}

	const char* Get_value_Char() const
	{
		return value.c_str();
	}

	bool Get_value_bool() const
	{
		return (value == "true" || value == "t");
	}

	bool Get_RO() const
	{
		return read_only;
	}

	void Set_value(const char* str)
	{
		if (!read_only)
			value = str;
	}

	int Get_min() const
	{
		return min_value;
	}

	int Get_max() const
	{
		return max_value;
	}

	j1Module* Callback;
	ARGUMENTS_TYPE Cvar_type;
};



#endif // !_CVAR_COM_H_

