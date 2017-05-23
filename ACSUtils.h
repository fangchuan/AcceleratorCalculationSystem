#pragma once

class ACSUtils
{
public:
	static bool CheckSoftwareLicense();
	static bool CheckSoftwareRegistry();
	~ACSUtils();

private:
	ACSUtils();
};
