/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2022 Ralf Rettig (www.personalfme.de)

This program is free software: you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>
*/
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <stdexcept>
#include "Groupalarm2LoginData.h"


/** @brief	Standard constructor.
*/
External::Groupalarm::CGroupalarm2LoginData::CGroupalarm2LoginData(void)
	: CGatewayLoginData(),
	isValid(false)
{
}

/** @brief	Destructor.
*/
External::Groupalarm::CGroupalarm2LoginData::~CGroupalarm2LoginData(void)
{
}

/** @brief		Provides a clone of the current object
*	@return									Clone of the current object
*	@exception								None
*	@remarks								None
*/
std::unique_ptr< External::CGatewayLoginData > External::Groupalarm::CGroupalarm2LoginData::Clone(void) const
{
	return std::make_unique<CGroupalarm2LoginData>(*this);
}

void External::Groupalarm::CGroupalarm2LoginData::Get(unsigned int& organizationId, std::string& apiToken, std::string& proxyAddress, unsigned short& proxyPort, std::string& proxyUserName, std::string& proxyPassword) const
{
	// check if the gateway data has been completely set
	if (!IsValid()) {
		throw std::runtime_error("The login data is not completely set.");
	}

	organizationId = CGroupalarm2LoginData::organizationId;
	apiToken = CGroupalarm2LoginData::apiToken;
	proxyAddress = CGroupalarm2LoginData::proxyAddress;
	proxyPort = CGroupalarm2LoginData::proxyPort;
	proxyUserName = CGroupalarm2LoginData::proxyUserName;
	proxyPassword = CGroupalarm2LoginData::proxyPassword;
}

void External::Groupalarm::CGroupalarm2LoginData::Set(const unsigned int& organizationId, const std::string& apiToken, const std::string& proxyAddress, const unsigned short& proxyPort, const std::string& proxyUserName, const std::string& proxyPassword)
{
	CGroupalarm2LoginData::organizationId = organizationId;
	CGroupalarm2LoginData::apiToken = apiToken;
	CGroupalarm2LoginData::proxyAddress = proxyAddress;
	CGroupalarm2LoginData::proxyPort = proxyPort;
	CGroupalarm2LoginData::proxyUserName = proxyUserName;
	CGroupalarm2LoginData::proxyPassword = proxyPassword;
	isValid = true;
}

/**	@brief		Determines if the current login data is valid
*	@return										True if the login data is valid, false otherwise
*	@exception									None
*	@remarks									None
*/
bool External::Groupalarm::CGroupalarm2LoginData::IsValid() const
{
	if (!IsBaseClassValid() || !isValid) {
		return false;
	}

	return true;
}

/**	@brief		Equality comparison method
*	@param		rhs								Object to be compared with the present object
*	@return										True if the objects are equal, false otherwise
*	@exception									None
*	@remarks									This method is required for the equality and unequality operators
*/
bool External::Groupalarm::CGroupalarm2LoginData::IsEqual(const CGatewayLoginData& rhs) const
{
	unsigned int rhsOganizationId;
	std::string rhsApiToken;
	std::string rhsProxyAddress;
	unsigned short rhsProxyPort;
	std::string rhsProxyUserName;
	std::string rhsProxyPassword;

	// compare the base objects
	if (!CGatewayLoginData::IsEqual(rhs)) {
		return false;
	}

	try {
		const CGroupalarm2LoginData& derivRhs = dynamic_cast<const CGroupalarm2LoginData&>(rhs);

		// checks if both datasets are either empty or non-empty
		if ((IsValid() && !derivRhs.IsValid()) || (!IsValid() && derivRhs.IsValid())) {
			return false;
		}

		// checks for equality of all components
		if (IsValid() && derivRhs.IsValid()) {
			derivRhs.Get(rhsOganizationId, rhsApiToken, rhsProxyAddress, rhsProxyPort, rhsProxyUserName, rhsProxyPassword);
			if (organizationId != rhsOganizationId) {
				return false;
			}
			if (apiToken != rhsApiToken) {
				return false;
			}
			if (proxyAddress != rhsProxyAddress) {
				return false;
			}
			if (proxyPort != rhsProxyPort) {
				return false;
			}
			if (proxyUserName != rhsProxyUserName) {
				return false;
			}
			if (proxyUserName != rhsProxyUserName) {
				return false;
			}
		}
	}
	catch (std::bad_cast e) {
		return false;
	}

	return true;
}

std::string External::Groupalarm::CGroupalarm2LoginData::GetProxyAddress() const
{
	return proxyAddress;
}


unsigned short External::Groupalarm::CGroupalarm2LoginData::GetProxyPort() const
{
	return proxyPort;
}

std::string External::Groupalarm::CGroupalarm2LoginData::GetProxyUserName() const
{
	return proxyUserName;
}

std::string External::Groupalarm::CGroupalarm2LoginData::GetProxyPassword() const
{
	return proxyPassword;
}

bool External::Groupalarm::CGroupalarm2LoginData::IsWithProxy() const
{
	return !proxyAddress.empty();
}

bool External::Groupalarm::CGroupalarm2LoginData::IsWithProxyWithUserNameAndPassword() const
{
	return IsWithProxy() && !proxyUserName.empty() && !proxyPassword.empty();
}

bool External::Groupalarm::CGroupalarm2LoginData::IsWithProxyWithUserNameOnly() const
{
	return IsWithProxy() && !proxyUserName.empty() && proxyPassword.empty();
}

std::string External::Groupalarm::CGroupalarm2LoginData::GetApiToken() const
{
	return apiToken;
}

unsigned int External::Groupalarm::CGroupalarm2LoginData::GetOrganizationId() const
{
	return organizationId;
}
