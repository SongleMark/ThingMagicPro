//============================================================================
// @file        : pubdataitem.h
// @Author      : terry.ye
// @Version     : 1.0.0.0 
// @Copyright   : Boingtech 2018
// @Description : Header for pub data item
//============================================================================
#ifndef __PUB_DATA_ITEM_H_
#define __PUB_DATA_ITEM_H_

#pragma region include
#include <string>
#include <map>
#include <vector>
#include <mutex>
#pragma endregion include

using namespace std;

/// @struct PubDataItem
/// A Key-Value Struct for  recorde the publish data
#pragma region PubDataItem
struct PubDataItem
{
	string tid; 			///< tid value
	string epc;				///< epc value
	string um;	 			///< um value
	string res; 			///< res value
	bool   status;			///< status,true is published,false is not.
	
	/// @fn PubDataItem
	/// constructor
	PubDataItem()
	{
		this->tid = "";
		this->epc = "";
		this->um = "";
		this->res = "";
		this->status = false;
	}
	
	/// @fn PubDataItem
	/// constructor
	PubDataItem(string strTid, string strEpc, string strUm,string strRes,bool st)
	{
		this->tid = strTid;
		this->epc = strEpc;
		this->um = strUm;
		this->res = strRes;
		this->status = st;
	}
		
	/// @fn operator =
	/// Overloaded function =
	PubDataItem& operator =(const PubDataItem&item)
	{
		this->tid = item.tid;
		this->epc = item.epc;
		this->um = item.um;
		this->res = item.res;
		this->status = item.status;
		return *this;
	}
		
	/// @fn IsValidate
	/// Judgment value is valid
	bool IsValidate()
	{
		return this->tid.empty() ? false : true;
	}
};
#pragma endregion PubDataItem

/// @class PubData
/// use to recorde data  for publish
class PubData
{
public:
	PubData();		///< constructor
	~PubData();		///< destructor 
	
	/// get PubDataItem from the map and vector
	/// @param key the string use as a key to find data in the map
	/// @return PubDataItem£¬if not find, the tid,epc is empty£¬also can use the function isvalidate to get the states.
	PubDataItem find_PubData(std::string const& key);
	
	/// add PubDataItem to the map and vector
	/// @param key the string use as a key add data to the map
	/// @param pubdataItem use as a value match the key add data to the map and vector
	/// @return void
	void add_PubData(std::string const& key, PubDataItem const& pubdataItem);
		
	map<string, int> m_PubDataIndexMap;		///< A Key-Value[tid,vector_index] Struct for  recorde the publish data index
	vector<PubDataItem> m_PubDataVct;		///< A Key-Value[vector_index,(epc,other,status)] Struct for  recorde the publish data
	mutex m_PubDataMutex;   				///< PubData Mutex
};
//PubData* g_PubData;
#endif /* __PUB_DATA_ITEM_H_ */
