#include "vpsndipassivetool.h"
#include <iostream>
#include <fstream>


NDIPassiveTool::NDIPassiveTool()
: InternalTrackingTool(),
m_SROMData(NULL),
m_SROMDataLength(0),
m_TrackingPriority(Dynamic),
m_PortHandle()
{
}


NDIPassiveTool::~NDIPassiveTool()
{
	if (m_SROMData != NULL)
	{
		delete[] m_SROMData;
		m_SROMData = NULL;
	}
}


bool NDIPassiveTool::LoadSROMFile(const char* filename)
{
	if (filename == NULL)
		return false;
	if (filename[0] == '\0')
		return false;

	m_File = filename;
	std::basic_ifstream<char> file;
	file.open(filename, std::ios::in | std::ios::binary); // open the file
	if (file.is_open() == false)
		return false;

	file.seekg (0, std::ios::end);                // get the length of the file
	unsigned int newLength = file.tellg();
	file.seekg (0, std::ios::beg);
	char* newData = new char [newLength];         // create a buffer to store the srom file
	file.read(newData, newLength);                // read the file into the buffer
	file.close();
	if (file.fail() == true)                      // reading of data unsuccessful?
	{
		delete[] newData;
		return false;
	}
	if (m_SROMData != NULL)                       // reading was successful, delete old data
		delete[] m_SROMData;
	m_SROMDataLength = newLength;                 // set member variables to new values
	m_SROMData = (unsigned char*) newData;
	this->Modified();
	return true;
}


const unsigned char* NDIPassiveTool::GetSROMData() const
{
	return m_SROMData;
}


unsigned int NDIPassiveTool::GetSROMDataLength() const
{
	return m_SROMDataLength;
}
