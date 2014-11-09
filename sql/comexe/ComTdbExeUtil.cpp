/**********************************************************************
// @@@ START COPYRIGHT @@@
//
// (C) Copyright 1998-2014 Hewlett-Packard Development Company, L.P.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// @@@ END COPYRIGHT @@@
**********************************************************************/
/* -*-C++-*-
****************************************************************************
*
* File:         ComTdbExeUtil.cpp
* Description:  
*
* Created:      5/6/98
* Language:     C++
*
*
*
*
****************************************************************************
*/

#include "ComTdbExeUtil.h"
#include "ComTdbCommon.h" 
#include "ComSmallDefs.h"

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtil
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtil::ComTdbExeUtil(Lng32 type,
			     char * query,
			     ULng32 querylen,
			     Int16 querycharset,
			     char * tableName,
			     ULng32 tableNameLen,
			     ex_expr * input_expr,
			     ULng32 input_rowlen,
			     ex_expr * output_expr,
			     ULng32 output_rowlen,
			     ex_expr_base * scan_expr,
			     ex_cri_desc * work_cri_desc,
			     const unsigned short work_atp_index,
			     ex_cri_desc * given_cri_desc,
			     ex_cri_desc * returned_cri_desc,
			     queue_index down,
			     queue_index up,
			     Lng32 num_buffers,
			     ULng32 buffer_size)
     : ComTdbGenericUtil(query, querylen, querycharset, tableName, tableNameLen,
			 input_expr, input_rowlen,
			 output_expr, output_rowlen,
			 work_cri_desc, work_atp_index,
			 given_cri_desc, returned_cri_desc,
			 down, up, 
			 num_buffers, buffer_size),
       type_(type),
       child_(NULL),
       scanExpr_(scan_expr),
       flags_(0),
       explOptionsStr_(NULL)
{
  setNodeType(ComTdb::ex_EXE_UTIL);
}

Long ComTdbExeUtil::pack(void * space)
{
  child_.pack(space);
  scanExpr_.pack(space);
  if (explOptionsStr_) 
    explOptionsStr_.pack(space);
  if (NEOCatalogName_) 
    NEOCatalogName_.pack(space);
  return ComTdbGenericUtil::pack(space);
}

Lng32 ComTdbExeUtil::unpack(void * base, void * reallocator)
{
  if(child_.unpack(base, reallocator)) return -1;
  if(scanExpr_.unpack(base, reallocator)) return -1;
  if(explOptionsStr_.unpack(base))
    return -1;
  if(NEOCatalogName_.unpack(base)) 
    return -1;
  return ComTdbGenericUtil::unpack(base, reallocator);
}

const ComTdb* ComTdbExeUtil::getChild(Int32 pos) const
{
  if (pos == 0)
    return child_;
  else
    return NULL;
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilDisplayExplain
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilDisplayExplain::ComTdbExeUtilDisplayExplain
(char * query,
 ULng32 querylen,
 Int16 querycharset,
 char * moduleName,
 char * stmtName,
 char optionX,
 ex_expr * input_expr,
 ULng32 input_rowlen,
 ex_expr * output_expr,
 ULng32 output_rowlen,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 Lng32 colDescSize,
 Lng32 outputRowSize,
  ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::DISPLAY_EXPLAIN_,
		     query, querylen, querycharset,
		     NULL, 0,
		     input_expr, input_rowlen,
		     output_expr, output_rowlen,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       moduleName_(moduleName),
       stmtName_(stmtName),
       colDescSize_(colDescSize),
       outputRowSize_(outputRowSize),
       flags_(0)
{
  setNodeType(ComTdb::ex_DISPLAY_EXPLAIN);

  setOptionX(optionX);
}

void ComTdbExeUtilDisplayExplain::setOptionX(char c)    // move from char to mask_
{
  flags_ &= OPTION_OFF;                                 // clear in case reused
  switch(c)
    {
      case 'e' : flags_ |= OPTION_E; break;             // expert mode
      case 'f' : flags_ |= OPTION_F; break;             // summary mode
      case 'm' : flags_ |= OPTION_M; break;             // machine readable mode
      case 'n' : flags_ |= OPTION_N; break;             // normal mode
      default  : assert(c == 'n');                      // always fail, input not supported
    }
  return;
}

Long ComTdbExeUtilDisplayExplain::pack(void * space)
{
  if (moduleName_) 
    moduleName_.pack(space);
  if (stmtName_) 
    stmtName_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilDisplayExplain::unpack(void * base, void * reallocator)
{
  if(moduleName_.unpack(base))
    return -1;
  if(stmtName_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilDisplayExplain::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[100];
      str_sprintf(buf, "\nFor ComTdbExeUtilDisplayExplain :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      char c = 'm';
      if (isOptionN()) {c = 'n';}
      else if (isOptionF()) {c = 'f';}
      else if (isOptionE()) {c = 'e';}
      str_sprintf(buf,"optionX_ = %c", c);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilDisplayExplainComplex
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilDisplayExplainComplex::ComTdbExeUtilDisplayExplainComplex
(Lng32 explainType,
 char * qry1, char * qry2, char * qry3, char * qry4,
 char * objectName,
 Lng32 objectNameLen,
 ex_expr * input_expr,
 ULng32 input_rowlen,
 ex_expr * output_expr,
 ULng32 output_rowlen,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::DISPLAY_EXPLAIN_COMPLEX_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     objectName, objectNameLen,
		     input_expr, input_rowlen,
		     output_expr, output_rowlen,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       explainType_(explainType),
       qry1_(qry1), qry2_(qry2), qry3_(qry3), qry4_(qry4),
       flags_(0)
{
  setNodeType(ComTdb::ex_DISPLAY_EXPLAIN_COMPLEX);
}

Long ComTdbExeUtilDisplayExplainComplex::pack(void * space)
{
  if (qry1_)
    qry1_.pack(space);

  if (qry2_)
    qry2_.pack(space);

  if (qry3_)
    qry3_.pack(space);

  if (qry4_)
    qry4_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilDisplayExplainComplex::unpack(void * base, void * reallocator)
{
  if (qry1_.unpack(base))
    return -1;
  if (qry2_.unpack(base))
    return -1;
  if (qry3_.unpack(base))
    return -1;
  if (qry4_.unpack(base))
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilDisplayExplainComplex::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];

      str_sprintf(buf, "\nFor ComTdbExeUtilDisplayExplainComplex :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
  
      if (qry1_)
	{
	  char query[400];
	  if (strlen(qry1_) > 390)
	    {
	      strncpy(query, qry1_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, qry1_);
	  
	  str_sprintf(buf,"Qry1 = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      
      if (qry2_)
	{
	  char query[400];
	  if (strlen(qry2_) > 390)
	    {
	      strncpy(query, qry2_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, qry2_);
	  
	  str_sprintf(buf,"Qry2 = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      
      if (qry3_)
	{
	  char query[400];
	  if (strlen(qry3_) > 390)
	    {
	      strncpy(query, qry3_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, qry3_);
	  
	  str_sprintf(buf,"Qry3 = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      
      if (qry4_)
	{
	  char query[400];
	  if (strlen(qry4_) > 390)
	    {
	      strncpy(query, qry4_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, qry4_);
	  
	  str_sprintf(buf,"Qry4 = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (objectName_)
	{
	  str_sprintf(buf,"ObjectName = %s ",getObjectName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }

  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilReorg
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilReorg::ComTdbExeUtilReorg(
                                       char * catName,
                                       ULng32 catNameLen,
                                       char * tableName,
				       ULng32 tableNameLen,
				       Int64 objectUID,
				       ObjectType ot,
                                       Lng32 numTables,
				       ex_expr * input_expr,
				       ULng32 input_rowlen,
				       ex_expr * output_expr,
				       ULng32 output_rowlen,
				       ex_cri_desc * work_cri_desc,
				       const unsigned short work_atp_index,
				       ex_cri_desc * given_cri_desc,
				       ex_cri_desc * returned_cri_desc,
				       queue_index down,
				       queue_index up,
				       Lng32 num_buffers,
				       ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::REORG_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     tableName, tableNameLen,
		     input_expr, input_rowlen,
		     output_expr, output_rowlen,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       catName_(catName),
       catNameLen_(catNameLen),
       reorgTableObjectUID_(objectUID),
       numTables_(numTables),
       ot_(ot),
       flags_(0),
       flags2_(0)
{
  setNodeType(ComTdb::ex_REORG);
}

void ComTdbExeUtilReorg::setParams(Lng32 concurrency, 
				   Lng32 firstPartn, Lng32 lastPartn,
				   Lng32 rate, 
				   Lng32 dslack, 
				   Lng32 islack,
				   Lng32 delay,
				   Lng32 priority,
				   NABoolean noDealloc,
				   char * partnName, UInt32 partnNameLen,
				   NABoolean displayOnly, 
				   NABoolean noOutputMsg,
				   NABoolean returnSummary,
				   NABoolean returnDetailOutput,
				   NABoolean getStatus,
				   NABoolean suspend,
                                   NABoolean stop,
				   NABoolean newReorg,
				   NABoolean doCheck,
				   NABoolean doReorg,
				   NABoolean reorgIfNeeded,
                                   NABoolean doCompact,
                                   NABoolean doOverride,
                                   Lng32 compressionType,
				   NABoolean updateReorgDB,
				   NABoolean useReorgDB,
				   NABoolean returnFast,
				   Lng32 rowsetSize,
				   Lng32 firstTable,
				   Lng32 lastTable,
				   char * whereClauseStr,
				   NABoolean generateMaintainCommands,
				   NABoolean showMaintainCommands,
				   NABoolean runMaintainCommands,
				   Lng32 maxMaintainTables,
				   NABoolean reorgCheckAll,
				   NABoolean generateCheckCommands,
				   Lng32 concurrentCheckSessions,
				   NABoolean continueOnError,
				   NABoolean systemObjectsOnly,
				   NABoolean debugOutput,
				   NABoolean initialize,
				   NABoolean reinitialize,
				   NABoolean drop,
				   NABoolean createView,
				   NABoolean dropView)
{
  concurrency_ = concurrency;
  firstPartn_ = firstPartn;
  lastPartn_ = lastPartn;
  rate_ = rate;
  dslack_ = dslack;
  islack_ = islack;
  delay_ = delay;
  priority_ = priority;
  partnName_ = partnName;
  partnNameLen_ = partnNameLen;
  setDisplayOnly(displayOnly);
  setNoOutputMsg(noOutputMsg);
  setReturnSummary(returnSummary);
  setReturnDetailOutput(returnDetailOutput);
  setGetStatus(getStatus);
  setSuspend(suspend);
  setStop(stop);
  setNewReorg(newReorg);
  setDoCheck(doCheck);
  setDoReorg(doReorg);
  setReorgIfNeeded(reorgIfNeeded);
  setNoDealloc(noDealloc);
  setDoCompact(doCompact);
  setDoOverride(doOverride);
  setReorgCheckAll(reorgCheckAll);
  compressionType_ = compressionType;
  setUpdateReorgDB(updateReorgDB);
  setUseReorgDB(useReorgDB);
  setReturnFast(returnFast);
  rowsetSize_ = rowsetSize;
  firstTable_ = firstTable;
  lastTable_ = lastTable;
  whereClauseStr_ = whereClauseStr;
  setGenerateMaintainCommands(generateMaintainCommands);
  setShowMaintainCommands(showMaintainCommands);
  setRunMaintainCommands(runMaintainCommands);
  maxMaintainTables_ = maxMaintainTables;
  setGenerateCheckCommands(generateCheckCommands);
  concurrentCheckSessions_ = concurrentCheckSessions;
  setContinueOnError(continueOnError);
  setSystemObjectsOnly(systemObjectsOnly);
  setDebugOutput(debugOutput);
  setInitializeDB(initialize);
  setReInitializeDB(reinitialize);
  setDropDB(drop);
  setCreateView(createView);
  setDropView(dropView);
}

void ComTdbExeUtilReorg::setLists(Queue* reorgPartnsSegmentNameList,
				  Queue* reorgPartnsNameList,
				  Queue* reorgPartnsGuaNameList,
                                  Queue* reorgPartnsCpuList,
                                  Queue* reorgPartnsDp2List,
				  Queue* reorgPartnsTableNumList,
				  Queue* reorgTableNamesList,
				  Queue* reorgTableObjectUIDList)
{
  reorgPartnsSegmentNameList_ = reorgPartnsSegmentNameList;
  reorgPartnsNameList_        = reorgPartnsNameList;
  reorgPartnsGuaNameList_     = reorgPartnsGuaNameList;
  reorgPartnsCpuList_         = reorgPartnsCpuList;
  reorgPartnsDp2List_         = reorgPartnsDp2List;
  reorgPartnsTableNumList_    = reorgPartnsTableNumList;
  reorgTableNamesList_        = reorgTableNamesList;
  reorgTableObjectUIDList_    = reorgTableObjectUIDList;
}

Long ComTdbExeUtilReorg::pack(void * space)
{
  reorgPartnsSegmentNameList_.pack(space);
  reorgPartnsNameList_.pack(space);
  reorgPartnsGuaNameList_.pack(space);
  reorgPartnsCpuList_.pack(space);
  reorgPartnsDp2List_.pack(space);
  reorgPartnsTableNumList_.pack(space);
  reorgTableNamesList_.pack(space);
  reorgTableObjectUIDList_.pack(space);

  if (partnName_)
    partnName_.pack(space);

  if (catName_)
    catName_.pack(space);

  if (whereClauseStr_)
    whereClauseStr_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilReorg::unpack(void * base, void * reallocator)
{
  if(reorgPartnsSegmentNameList_.unpack(base, reallocator)) return -1;
  if(reorgPartnsNameList_.unpack(base, reallocator)) return -1;
  if(reorgPartnsGuaNameList_.unpack(base, reallocator)) return -1;
  if(reorgPartnsCpuList_.unpack(base, reallocator)) return -1;
  if(reorgPartnsDp2List_.unpack(base, reallocator)) return -1;
  if(reorgPartnsTableNumList_.unpack(base, reallocator)) return -1;
  if(reorgTableNamesList_.unpack(base, reallocator)) return -1;
  if(reorgTableObjectUIDList_.unpack(base, reallocator)) return -1;

  if(partnName_.unpack(base))
    return -1;

  if(catName_.unpack(base))
    return -1;

  if (whereClauseStr_.unpack(base))
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilReorg::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilReorg :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      str_sprintf(buf,"concurrency_ = %d", concurrency_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      str_sprintf(buf,"rate_ = %d, dslack_ = %d, islack_ = %d, priority_ = %d",
		  rate_, dslack_, islack_, priority_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

      if (getDoCheck() && (getWhereClauseStr() != NULL))
        {
	  char query[400];
	  if (strlen(getWhereClauseStr()) > 390)
	    {
	      strncpy(query, getWhereClauseStr(), 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, getWhereClauseStr());

	  str_sprintf(buf,"Where Pred = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

	}
    } 
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

//////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilReplicate
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilReplicate::ComTdbExeUtilReplicate(
     char * sourceName,
     UInt32 sourceLen,
     char * targetName,
     UInt32 targetLen,
     char * sourceSchema,
     UInt32 sourceSchemaLen,
     char * targetSystem,
     UInt32 targetSystemLen,
     char * targetSchema,
     UInt32 targetSchemaLen,
     char * ddlInputStr,
     UInt32 ddlInputStrLen,
     char * schGetTablesStmt,
     UInt32 schGetTablesStmtLen,
     char * schTableReplStmt,
     UInt32 schTableReplStmtLen,
     char * indexReplStmt,
     UInt32 indexReplStmtLen,
     char * uniqImplIndexReplStmt,
     UInt32 uniqImplIndexReplStmtLen,
     char * controlQueryId,
     UInt32 controlQueryIdLen,  
     char * srcCatName,
     char * tgtCatName,
     ObjectType ot,
     ex_expr * input_expr,
     UInt32 input_rowlen,
     ex_expr * output_expr,
     UInt32 output_rowlen,
     ex_cri_desc * work_cri_desc,
     const UInt16 work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     UInt32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::REPLICATE_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     input_expr, input_rowlen,
		     output_expr, output_rowlen,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       sourceName_(sourceName), sourceLen_(sourceLen),
       targetName_(targetName), targetLen_(targetLen),
       sourceSchema_(sourceSchema), sourceSchemaLen_(sourceSchemaLen),
       targetSystem_(targetSystem), targetSystemLen_(targetSystemLen),
       targetSchema_(targetSchema), targetSchemaLen_(targetSchemaLen),
       ddlInputStr_(ddlInputStr), ddlInputStrLen_(ddlInputStrLen),
       schGetTablesStmt_(schGetTablesStmt), schGetTablesStmtLen_(schGetTablesStmtLen),
       schTableReplStmt_(schTableReplStmt), schTableReplStmtLen_(schTableReplStmtLen),
       indexReplStmt_(indexReplStmt), indexReplStmtLen_(indexReplStmtLen),
       uniqImplIndexReplStmt_(uniqImplIndexReplStmt), uniqImplIndexReplStmtLen_(uniqImplIndexReplStmtLen),
       controlQueryId_(controlQueryId), controlQueryIdLen_(controlQueryIdLen),
       srcCatName_(srcCatName), tgtCatName_(tgtCatName),
       roleName_(NULL),
       numSrcPartns_(0),
       srcImpUnqIxPos_(0),
       portNum_(0),
       numRetries_(0),
       ot_(ot),
       flags_(0),
       flags2_(0),
       compressionType_(COM_SOURCE_COMPRESSION),
       diskPool_(1),
       formatFlags_(0)
{
  setNodeType(ComTdb::ex_REPLICATE);
  if (!targetSchema_.isNull())
     setIncrementalDisallowed(TRUE);
  forceError_[0] = 0;
}

void ComTdbExeUtilReplicate::setParams(Lng32 concurrency, 
				       Lng32 rate, 
				       Lng32 priority,
				       Lng32 delay,
				       Int64 srcObjectUid,
				       NABoolean compress,
				       NABoolean purgedataTgt,
				       NABoolean incremental,
				       NABoolean validateTgtDDL,
				       NABoolean validateData,
				       NABoolean createTgtDDL,
				       NABoolean tgtObjsOnline,
				       NABoolean tgtObjsOffline,
				       NABoolean tgtObjsAudited,
				       NABoolean tgtObjsUnaudited,
				       NABoolean returnPartnDetails,
				       NABoolean transform,
				       NABoolean getStatus,
				       NABoolean suspend,
				       NABoolean resume,
				       NABoolean abort,
				       NABoolean authid,
				       NABoolean copyBothDataAndStats,
				       NABoolean displayInternalCmd)
{
  concurrency_ = concurrency;
  rate_ = rate;
  priority_ = priority;
  delay_ = delay;
  srcObjectUid_ = srcObjectUid;
  setCompress(compress);
  setPurgedataTgt(purgedataTgt);
  setIncremental(incremental);
  setValidateTgtDDL(validateTgtDDL);
  setValidateData(validateData);
  setCreateTgtDDL(createTgtDDL);
  setAlterTgtOnline(tgtObjsOnline);
  setAlterTgtOffline(tgtObjsOffline);
  setAlterTgtAudited(tgtObjsAudited);
  setAlterTgtUnaudited(tgtObjsUnaudited);
  setReturnPartnDetails(returnPartnDetails);
  setTransform(transform);
  setGetStatus(getStatus);
  setSuspend(suspend);
  setResume(resume);
  setAbort(abort);
  setReplicateAuthid(authid);
  setCopyBothDataAndStats(copyBothDataAndStats);
  setDisplayInternalCmd(displayInternalCmd);
}

void ComTdbExeUtilReplicate::setLists(Queue* replIndexList,
				      Queue* replIndexTypeList,
				      Queue* replPartnsSegmentNameList,
				      Queue* replPartnsNameList,
				      Queue* replPartnsGuaNameList,
				      Queue* replPartnsCpuList,
				      Queue* replPartnsDp2List)
{
  replIndexList_             = replIndexList;
  replIndexTypeList_         = replIndexTypeList;
  replPartnsSegmentNameList_ = replPartnsSegmentNameList;
  replPartnsNameList_        = replPartnsNameList;
  replPartnsGuaNameList_     = replPartnsGuaNameList;
  replPartnsCpuList_         = replPartnsCpuList;
  replPartnsDp2List_         = replPartnsDp2List;
}

Long ComTdbExeUtilReplicate::pack(void * space)
{
  if (sourceName_) 
    sourceName_.pack(space);
  if (targetName_) 
    targetName_.pack(space);
  if (sourceSchema_) 
    sourceSchema_.pack(space);
  if (targetSystem_) 
    targetSystem_.pack(space);
  if (ddlInputStr_) 
    ddlInputStr_.pack(space);
  if (ipAddr_) 
    ipAddr_.pack(space);
  if (schGetTablesStmt_)
    schGetTablesStmt_.pack(space);
  if (schTableReplStmt_)
    schTableReplStmt_.pack(space);
  if (indexReplStmt_)
    indexReplStmt_.pack(space);
  if (uniqImplIndexReplStmt_)
    uniqImplIndexReplStmt_.pack(space);
  if (controlQueryId_)
    controlQueryId_.pack(space);
  if (srcCatName_)
    srcCatName_.pack(space);
  if (tgtCatName_)
    tgtCatName_.pack(space);
  if (roleName_)
    roleName_.pack(space);
  if (mgbltyCatalogName_)
    mgbltyCatalogName_.pack(space);
  if (testTgtMgbltyCatalogName_)
    testTgtMgbltyCatalogName_.pack(space);

  replIndexList_.pack(space);
  replIndexTypeList_.pack(space);
  replPartnsSegmentNameList_.pack(space);
  replPartnsNameList_.pack(space);
  replPartnsGuaNameList_.pack(space);
  replPartnsCpuList_.pack(space);
  replPartnsDp2List_.pack(space);
  if (!targetSchema_.isNull())
     targetSchema_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilReplicate::unpack(void * base, void * reallocator)
{
  if (sourceName_.unpack(base)) return -1;
  if (targetName_.unpack(base)) return -1;
  if (sourceSchema_.unpack(base)) return -1;
  if (targetSystem_.unpack(base)) return -1;
  if (ddlInputStr_.unpack(base)) return -1;
  if (ipAddr_.unpack(base)) return -1;
  if (schGetTablesStmt_.unpack(base)) return -1;
  if (schTableReplStmt_.unpack(base)) return -1;
  if (indexReplStmt_.unpack(base)) return -1;
  if (uniqImplIndexReplStmt_.unpack(base)) return -1;
  if (controlQueryId_.unpack(base)) return -1;
  if (srcCatName_.unpack(base)) return -1;
  if (tgtCatName_.unpack(base)) return -1;
  if (roleName_.unpack(base)) return -1;
  if (mgbltyCatalogName_.unpack(base)) return -1;
  if (testTgtMgbltyCatalogName_.unpack(base)) return -1;

  if(replIndexList_.unpack(base, reallocator)) return -1;
  if(replIndexTypeList_.unpack(base, reallocator)) return -1;
  if(replPartnsSegmentNameList_.unpack(base, reallocator)) return -1;
  if(replPartnsNameList_.unpack(base, reallocator)) return -1;
  if(replPartnsGuaNameList_.unpack(base, reallocator)) return -1;
  if(replPartnsCpuList_.unpack(base, reallocator)) return -1;
  if(replPartnsDp2List_.unpack(base, reallocator)) return -1;
  if (targetSchema_.unpack(base)) return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilReplicate::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[100];
      str_sprintf(buf, "\nFor ComTdbExeUtilReplicate :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if ((char *)sourceName_ != (char *)NULL)
	{
	  str_sprintf(buf,"SourceName = %s ",sourceName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      str_sprintf(buf,"rate_ = %d, priority_ = %d",
		  rate_, priority_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

const char *ComTdbExeUtilReplicate::getObjectType(ObjectType ot)
{
   switch (ot)
   {
     case TABLE_:
        return "TABLE";
     case INDEX_:
        return "INDEX";
     case MV_:
        return "MV";
     case SCHEMA_:
        return "SCHEMA";
     case SCHEMA_DDL_:
        return "SCHEMA_DDL";
     case SCHEMA_STATS_:
        return "SCHEMA_STATS";
     case OBJECT_DDL_:
        return "OBJECT_DDL";
     case REPL_RECOVER_:
        return "REPL_RECOVER";
     case REPL_STATUS_:
        return "REPL_STATUS";
     case REPL_ABORT_:
        return "REPL_ABORT";
     case REPL_INITIALIZE_: 
        return "REPL_INITIALIZE";
     case REPL_AUTHORIZATION_:
        return "REPL_AUTHORIZATION";
     default:
        return "UNKNOWN";
  }
}

ComTdbExeUtilReplicate::ObjectType ComTdbExeUtilReplicate::getObjectTypeForStats()
{
   if (initializeReplicate() || reinitializeReplicate() || dropReplicate() ||
      getCleanup() ||
      addConfig() || removeConfig())
      return REPL_INITIALIZE_;
   else if (recoverTarget() || recoverSource())
      return REPL_RECOVER_;
   else if (replicateSchemaDDL())
      return SCHEMA_DDL_;
   else if (replicateSchemaStats())  
      return SCHEMA_STATS_; 
   else if (schemaRepl() || inListRepl())
      return SCHEMA_;
   else if (createTgtDDL())
      return OBJECT_DDL_;
   else if (getStatus())
      return REPL_STATUS_;
   else if (getAbort())
      return REPL_ABORT_;
   else if (replicateAuthid())
      return REPL_AUTHORIZATION_;
   else
      return ot_;
}

const char *ComTdbExeUtilReplicate::getReplType(ReplType replType)
{
   switch (replType)
   {
     case FULL_:
        return "FULL";
     case INCREMENTAL_:
        return "INCREMENTAL";
     default:
        return "UNKNOWN";
   }
}



//////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilMaintainObject
//
//////////////////////////////////////////////////////////////////////////
ComTdbExeUtilMaintainObject::ComTdbExeUtilMaintainObject(
     char * objectName,
     ULng32 objectNameLen,
     char * schemaName,
     ULng32 schemaNameLen,
     UInt16 ot,
     char * parentTableName,
     ULng32 parentTableNameLen,
     ex_expr * input_expr,
     ULng32 input_rowlen,
     ex_expr * output_expr,
     ULng32 output_rowlen,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::MAINTAIN_OBJECT_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     objectName, objectNameLen,
		     input_expr, input_rowlen,
		     output_expr, output_rowlen,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       ot_(ot),
       schemaName_(schemaName),
       schemaNameLen_(schemaNameLen),
       parentTableName_(parentTableName),
       parentTableNameLen_(parentTableNameLen),
       flags_(0),
       controlFlags_(0),
       controlFlags2_(0),
       formatFlags_(0),
       maintainedTableCreateTime_(0),
       parentTableObjectUID_(0),
       from_(0),
       to_(0),
       flags2_(0)
{
  setNodeType(ComTdb::ex_MAINTAIN_OBJECT);
}

Long ComTdbExeUtilMaintainObject::pack(void * space)
{

  reorgTableOptions_.pack(space);
  reorgIndexOptions_.pack(space);
  updStatsTableOptions_.pack(space);
  updStatsMvlogOptions_.pack(space);
  updStatsMvsOptions_.pack(space);
  updStatsMvgroupOptions_.pack(space);
  refreshMvgroupOptions_.pack(space);
  refreshMvsOptions_.pack(space);
  reorgMvgroupOptions_.pack(space);
  reorgMvsOptions_.pack(space);
  reorgMvsIndexOptions_.pack(space);
  cleanMaintainCITOptions_.pack(space);

  indexList_.pack(space);
  refreshMvgroupList_.pack(space);
  refreshMvsList_.pack(space);
  reorgMvgroupList_.pack(space);
  reorgMvsList_.pack(space);
  reorgMvsIndexList_.pack(space);
  updStatsMvgroupList_.pack(space);
  updStatsMvsList_.pack(space);
  multiTablesNamesList_.pack(space);
  multiTablesCreateTimeList_.pack(space);
  skippedMultiTablesNamesList_.pack(space);

  if (parentTableName_) 
    parentTableName_.pack(space);
  if (schemaName_)
    schemaName_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilMaintainObject::unpack(void * base, void * reallocator)
{
  if(reorgTableOptions_.unpack(base)) return -1;
  if(reorgIndexOptions_.unpack(base)) return -1;
  if(updStatsTableOptions_.unpack(base)) return -1;
  if(updStatsMvlogOptions_.unpack(base)) return -1;
  if(updStatsMvsOptions_.unpack(base)) return -1;
  if(updStatsMvgroupOptions_.unpack(base)) return -1;
  if(refreshMvgroupOptions_.unpack(base)) return -1;
  if(refreshMvsOptions_.unpack(base)) return -1;
  if(reorgMvgroupOptions_.unpack(base)) return -1;
  if(reorgMvsOptions_.unpack(base)) return -1;
  if(reorgMvsIndexOptions_.unpack(base)) return -1;
  if(cleanMaintainCITOptions_.unpack(base)) return -1;

  if(indexList_.unpack(base, reallocator)) return -1;
  if(refreshMvgroupList_.unpack(base, reallocator)) return -1;
  if(refreshMvsList_.unpack(base, reallocator)) return -1;
  if(reorgMvgroupList_.unpack(base, reallocator)) return -1;
  if(reorgMvsList_.unpack(base, reallocator)) return -1;
  if(reorgMvsIndexList_.unpack(base, reallocator)) return -1;
  if(updStatsMvgroupList_.unpack(base, reallocator)) return -1;
  if(updStatsMvsList_.unpack(base, reallocator)) return -1;
  if(multiTablesNamesList_.unpack(base, reallocator)) return -1;
  if(multiTablesCreateTimeList_.unpack(base, reallocator)) return -1;
  if(skippedMultiTablesNamesList_.unpack(base, reallocator)) return -1;

  if(parentTableName_.unpack(base)) return -1;
  if(schemaName_.unpack(base)) return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilMaintainObject::setParams(NABoolean reorgTable,
					   NABoolean reorgIndex,
					   NABoolean updStatsTable,
					   NABoolean updStatsMvlog,
					   NABoolean updStatsMvs,
					   NABoolean updStatsMvgroup,
					   NABoolean refreshMvgroup,
					   NABoolean refreshMvs,
 					   NABoolean reorgMvgroup,
					   NABoolean reorgMvs,
					   NABoolean reorgMvsIndex,
 					   NABoolean continueOnError,
					   NABoolean cleanMaintainCIT,
					   NABoolean getSchemaLabelStats, 
					   NABoolean getLabelStats,
					    NABoolean getTableLabelStats,
					   NABoolean getIndexLabelStats,
					   NABoolean getLabelStatsIncIndexes,
					   NABoolean getLabelStatsIncInternal,
				           NABoolean getLabelStatsIncRelated
					    )
{
  setReorgTable(reorgTable);
  setReorgIndex(reorgIndex);
  setUpdStatsTable(updStatsTable);
  setUpdStatsMvlog(updStatsMvlog);
  setUpdStatsMvs(updStatsMvs);
  setUpdStatsMvgroup(updStatsMvgroup);
  setRefreshMvgroup(refreshMvgroup);
  setRefreshMvs(refreshMvs);
  setReorgMvgroup(reorgMvgroup);
  setReorgMvs(reorgMvs);
  setReorgMvsIndex(reorgMvsIndex);
  setContinueOnError(continueOnError);
  setCleanMaintainCIT(cleanMaintainCIT);
  setSchemaLabelStats(getSchemaLabelStats); 
  setTableLabelStats(getTableLabelStats); 
  setIndexLabelStats(getIndexLabelStats);
  setLabelStatsIncIndexes(getLabelStatsIncIndexes);
  setLabelStatsIncInternal(getLabelStatsIncInternal);
  setLabelStatsIncRelated(getLabelStatsIncRelated);

  
}

void ComTdbExeUtilMaintainObject::setOptionsParams
(char* reorgTableOptions,
 char* reorgIndexOptions,
 char* updStatsTableOptions,
 char* updStatsMvlogOptions,
 char* updStatsMvsOptions,
 char* updStatsMvgroupOptions,
 char* refreshMvgroupOptions,
 char* refreshMvsOptions,
 char* reorgMvgroupOptions,
 char* reorgMvsOptions,
 char* reorgMvsIndexOptions,
 char* cleanMaintainCITOptions)
{
  reorgTableOptions_ = reorgTableOptions;
  reorgIndexOptions_ = reorgIndexOptions;
  updStatsTableOptions_ = updStatsTableOptions;
  updStatsMvlogOptions_ = updStatsMvlogOptions;
  updStatsMvsOptions_ = updStatsMvsOptions;
  updStatsMvgroupOptions_ = updStatsMvgroupOptions;
  refreshMvgroupOptions_ = refreshMvgroupOptions;
  refreshMvsOptions_ = refreshMvsOptions;
  reorgMvgroupOptions_ = reorgMvgroupOptions;
  reorgMvsOptions_ = reorgMvsOptions;
  reorgMvsIndexOptions_ = reorgMvsIndexOptions;
  cleanMaintainCITOptions_ = cleanMaintainCITOptions;
}

void ComTdbExeUtilMaintainObject::setLists(Queue* indexList,
					   Queue* refreshMvgroupList,
					   Queue* refreshMvsList,
					   Queue* reorgMvgroupList,
					   Queue* reorgMvsList,
					   Queue* reorgMvsIndexList,
					   Queue* updStatsMvgroupList,
					   Queue* updStatsMvsList,
					   Queue* multiTablesNamesList,
					   Queue* skippedMultiTablesNamesList)
{
  indexList_    = indexList;
  refreshMvgroupList_ = refreshMvgroupList;
  refreshMvsList_    = refreshMvsList;
  reorgMvgroupList_ = reorgMvgroupList;
  reorgMvsList_      = reorgMvsList;
  reorgMvsIndexList_ = reorgMvsIndexList;
  updStatsMvgroupList_ = updStatsMvgroupList;
  updStatsMvsList_      = updStatsMvsList;
  multiTablesNamesList_ = multiTablesNamesList;
  skippedMultiTablesNamesList_ = skippedMultiTablesNamesList;
 }

void ComTdbExeUtilMaintainObject::setControlParams
(NABoolean disableReorgTable, 
 NABoolean enableReorgTable, 
 NABoolean disableReorgIndex,
 NABoolean enableReorgIndex,
 NABoolean disableUpdStatsTable,
 NABoolean enableUpdStatsTable,
 NABoolean disableUpdStatsMvs,
 NABoolean enableUpdStatsMvs,
 NABoolean disableRefreshMvs,
 NABoolean enableRefreshMvs,
 NABoolean disableReorgMvs,
 NABoolean enableReorgMvs,
 NABoolean resetReorgTable,
 NABoolean resetUpdStatsTable,
 NABoolean resetUpdStatsMvs,
 NABoolean resetRefreshMvs,
 NABoolean resetReorgMvs,
 NABoolean resetReorgIndex,
 NABoolean enableUpdStatsMvlog,
 NABoolean disableUpdStatsMvlog,
 NABoolean resetUpdStatsMvlog,
 NABoolean enableReorgMvsIndex,
 NABoolean disableReorgMvsIndex,
 NABoolean resetReorgMvsIndex,
 NABoolean enableRefreshMvgroup,
 NABoolean disableRefreshMvgroup,
 NABoolean resetRefreshMvgroup,
 NABoolean enableReorgMvgroup,
 NABoolean disableReorgMvgroup,
 NABoolean resetReorgMvgroup,
 NABoolean enableUpdStatsMvgroup,
 NABoolean disableUpdStatsMvgroup,
 NABoolean resetUpdStatsMvgroup,
 NABoolean enableTableLabelStats,
 NABoolean disableTableLabelStats,
 NABoolean resetTableLabelStats,
 NABoolean enableIndexLabelStats,
 NABoolean disableIndexLabelStats,
 NABoolean resetIndexLabelStats
)
{
  setDisableReorgTable(disableReorgTable);
  setDisableReorgIndex(disableReorgIndex);
  setDisableUpdStatsTable(disableUpdStatsTable);
  setDisableUpdStatsMvs(disableUpdStatsMvs);
  setDisableRefreshMvs(disableRefreshMvs);
  setDisableReorgMvs(disableReorgMvs);
  setEnableReorgTable(enableReorgTable);
  setEnableReorgIndex(enableReorgIndex);
  setEnableUpdStatsTable(enableUpdStatsTable);
  setEnableUpdStatsMvs(enableUpdStatsMvs);
  setEnableRefreshMvs(enableRefreshMvs);
  setEnableReorgMvs(enableReorgMvs);
  setResetReorgTable(resetReorgTable);
  setResetUpdStatsTable(resetUpdStatsTable);
  setResetUpdStatsMvs(resetUpdStatsMvs);
  setResetRefreshMvs(resetRefreshMvs);
  setResetReorgMvs(resetReorgMvs);
  setResetReorgIndex(resetReorgIndex);
  setEnableUpdStatsMvlog(enableUpdStatsMvlog);
  setDisableUpdStatsMvlog(disableUpdStatsMvlog);
  setResetUpdStatsMvlog(resetUpdStatsMvlog);
  setEnableReorgMvsIndex(enableReorgMvsIndex);
  setDisableReorgMvsIndex(disableReorgMvsIndex);
  setResetReorgMvsIndex(resetReorgMvsIndex);
  setEnableRefreshMvgroup(enableRefreshMvgroup);
  setDisableRefreshMvgroup(disableRefreshMvgroup);
  setResetRefreshMvgroup(resetRefreshMvgroup);
  setEnableReorgMvgroup(enableReorgMvgroup);
  setDisableReorgMvgroup(disableReorgMvgroup);
  setResetReorgMvgroup(resetReorgMvgroup);
  setEnableUpdStatsMvgroup(enableUpdStatsMvgroup);
  setDisableUpdStatsMvgroup(disableUpdStatsMvgroup);
  setResetUpdStatsMvgroup(resetUpdStatsMvgroup);
  setEnableTableLabelStats(enableTableLabelStats);
  setDisableTableLabelStats(disableTableLabelStats);
  setResetTableLabelStats(resetTableLabelStats);
  setEnableIndexLabelStats(enableIndexLabelStats);
  setDisableIndexLabelStats(disableIndexLabelStats);
  setResetIndexLabelStats(resetIndexLabelStats);

}

void ComTdbExeUtilMaintainObject::displayContents(Space * space,
						 ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilMaintainObject :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilLoadVolatileTable
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilLoadVolatileTable::ComTdbExeUtilLoadVolatileTable
(char * tableName,
 ULng32 tableNameLen,
 char * insertQuery,
 char * updStatsQuery,
 Int16 queryCharSet,
 Int64 threshold,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::LOAD_VOLATILE_TABLE_,
		     NULL, 0, queryCharSet/*for insertQuery & updStatsQuery*/,
		     tableName, tableNameLen,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       insertQuery_(insertQuery),
       updStatsQuery_(updStatsQuery),
       threshold_(threshold),
       flags_(0)
{
  setNodeType(ComTdb::ex_LOAD_VOLATILE_TABLE);
}

Long ComTdbExeUtilLoadVolatileTable::pack(void * space)
{
  if (insertQuery_) 
    insertQuery_.pack(space);
  if (updStatsQuery_) 
    updStatsQuery_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilLoadVolatileTable::unpack(void * base, void * reallocator)
{
  if(insertQuery_.unpack(base))
    return -1;
  if(updStatsQuery_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilLoadVolatileTable::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilLoadVolatileTable :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (insertQuery_)
	{
	  char query[400];
	  if (strlen(insertQuery_) > 390)
	    {
	      strncpy(query, insertQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, insertQuery_);

	  str_sprintf(buf,"Insert Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (updStatsQuery_)
	{
	  char query[400];
	  if (strlen(updStatsQuery_) > 390)
	    {
	      strncpy(query, updStatsQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, updStatsQuery_);

	  str_sprintf(buf,"UpdStats Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      str_sprintf(buf,"Threshold = %Ld ", threshold_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilCleanupVolatileTables
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilCleanupVolatileTables::ComTdbExeUtilCleanupVolatileTables
(char * catName,
 ULng32 catNameLen,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::CLEANUP_VOLATILE_SCHEMA_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     catName, catNameLen,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0)
{
  setNodeType(ComTdb::ex_CLEANUP_VOLATILE_TABLES);
}

void ComTdbExeUtilCleanupVolatileTables::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilCleanupVolatileTables :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetVotalileInfo
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetVolatileInfo::ComTdbExeUtilGetVolatileInfo
(
 char * param1,
 char * param2,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::GET_VOLATILE_INFO,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       param1_(param1), param2_(param2)
{
  setNodeType(ComTdb::ex_GET_VOLATILE_INFO);
}

Long ComTdbExeUtilGetVolatileInfo::pack(void * space)
{
  if (param1_) 
    param1_.pack(space);
  if (param2_) 
    param2_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilGetVolatileInfo::unpack(void * base, void * reallocator)
{
  if(param1_.unpack(base))
    return -1;
  if(param2_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilGetVolatileInfo::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetVotalileInfo :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetErrorInfo
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetErrorInfo::ComTdbExeUtilGetErrorInfo
(
 Lng32 errNum,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::GET_ERROR_INFO_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       errNum_(errNum)
{
  setNodeType(ComTdb::ex_GET_ERROR_INFO);
}

void ComTdbExeUtilGetErrorInfo::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetErrorInfo :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      str_sprintf(buf,"ErrorNum = %d ", errNum_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

//LCOV_EXCL_START
///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetVotalileInfo
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetFormattedDiskStats::ComTdbExeUtilGetFormattedDiskStats
(
 char * diskStatsStr,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::GET_FORMATTED_DISK_STATS_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       diskStatsStr_(diskStatsStr)
{
  setNodeType(ComTdb::ex_GET_FORMATTED_DISK_STATS);
}

Long ComTdbExeUtilGetFormattedDiskStats::pack(void * space)
{
  if (diskStatsStr_) 
    diskStatsStr_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilGetFormattedDiskStats::unpack(void * base, void * reallocator)
{
  if(diskStatsStr_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilGetFormattedDiskStats::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[400];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetFormattedDiskStats :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getDiskStatsStr() != NULL)
	{
	  str_sprintf(buf,"diskStatsStr_ = %s ",getDiskStatsStr());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}
//LCOV_EXCL_STOP
///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilCreateTableAs
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilCreateTableAs::ComTdbExeUtilCreateTableAs
(char * tableName,
 ULng32 tableNameLen,
 char * ctQuery,
 char * siQuery,
 char * viQuery,
 char * usQuery,
 Int64 threshold,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::CREATE_TABLE_AS_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     tableName, tableNameLen,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       ctQuery_(ctQuery), 
       siQuery_(siQuery), 
       viQuery_(viQuery),
       usQuery_(usQuery),
       threshold_(threshold),
       flags_(0)
{
  setNodeType(ComTdb::ex_CREATE_TABLE_AS);
}

Long ComTdbExeUtilCreateTableAs::pack(void * space)
{
  if (ctQuery_) 
    ctQuery_.pack(space);
  if (siQuery_) 
    siQuery_.pack(space);
  if (viQuery_) 
    viQuery_.pack(space);
  if (usQuery_) 
    usQuery_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilCreateTableAs::unpack(void * base, void * reallocator)
{
  if(ctQuery_.unpack(base))
    return -1;
  if(siQuery_.unpack(base))
    return -1;
  if(viQuery_.unpack(base))
    return -1;
  if(usQuery_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilCreateTableAs::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilCreateTableAs :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (ctQuery_)
	{
	  char query[400];
	  if (strlen(ctQuery_) > 390)
	    {
	      strncpy(query, ctQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, ctQuery_);

	  str_sprintf(buf,"Create Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (siQuery_)
	{
	  char query[400];
	  if (strlen(siQuery_) > 390)
	    {
	      strncpy(query, siQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, siQuery_);

	  str_sprintf(buf,"Sidetree Insert Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (viQuery_)
	{
	  char query[400];
	  if (strlen(viQuery_) > 390)
	    {
	      strncpy(query, viQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, viQuery_);

	  str_sprintf(buf,"VSBB Insert Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (usQuery_)
	{
	  char query[400];
	  if (strlen(usQuery_) > 390)
	    {
	      strncpy(query, usQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, usQuery_);

	  str_sprintf(buf,"UpdStats Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}




///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilFastDelete
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilFastDelete::ComTdbExeUtilFastDelete(
     char * tableName,
     ULng32 tableNameLen,
     char * primaryPartnLoc,
     Queue * indexList,
     char * stmt,
     ULng32 stmtLen,
     Lng32 numEsps,
     Int64 objectUID,
     Lng32 numLOBs,
     char * lobNumArray,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size,
     NABoolean isHiveTruncate,
     char * hiveTableLocation,
     char * hiveHostName,
     Lng32 hivePortNum)
     : ComTdbExeUtil(ComTdbExeUtil::FAST_DELETE_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     tableName, tableNameLen,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       indexList_(indexList),
       purgedataStmt_(stmt),
       purgedataStmtLen_(stmtLen),
       primaryPartnLoc_(primaryPartnLoc),
       numEsps_(numEsps),
       objectUID_(objectUID),
       numLOBs_(numLOBs),
       lobNumArray_(lobNumArray),
       hiveTableLocation_(hiveTableLocation),
       hiveHdfsHost_(hiveHostName),
       hiveHdfsPort_(hivePortNum)
{
  setIsHiveTruncate(isHiveTruncate);
  setNodeType(ComTdb::ex_FAST_DELETE);
}

Long ComTdbExeUtilFastDelete::pack(void * space)
{
  indexList_.pack(space);

  if (purgedataStmt_) 
    purgedataStmt_.pack(space);

  if (primaryPartnLoc_)
    primaryPartnLoc_.pack(space);

  if (lobNumArray_) 
    lobNumArray_.pack(space);

  if (hiveTableLocation_)
    hiveTableLocation_.pack(space);

  if (hiveHdfsHost_)
    hiveHdfsHost_.pack(space);


  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilFastDelete::unpack(void * base, void * reallocator)
{
  if(indexList_.unpack(base, reallocator)) return -1;

  if (purgedataStmt_.unpack(base)) 
    return -1;

  if (primaryPartnLoc_.unpack(base))
    return -1;

  if(lobNumArray_.unpack(base)) 
    return -1;

  if(hiveTableLocation_.unpack(base))
      return -1;

  if(hiveHdfsHost_.unpack(base))
      return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

short ComTdbExeUtilFastDelete::getLOBnum(short i)
{
  if ((i > numLOBs_) || (i <= 0))
    return -1;

  short lobNum = *((short*)&getLOBnumArray()[2*(i-1)]);

  return lobNum;
}

void ComTdbExeUtilFastDelete::displayContents(Space * space,
					      ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[500];
      str_sprintf(buf, "\nFor ComTdbExeUtilFastDelete :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));
	}

      if (purgedataStmt_)
	{
	  str_sprintf(buf,"purgedataStmt_ = %s ", purgedataStmt());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));
	}

      if (numLOBs_ > 0)
	{
	  str_sprintf(buf, "numLOBs_ = %d ", numLOBs_);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));
	}
    }

  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }

}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetStatistics
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetStatistics::ComTdbExeUtilGetStatistics
(
     char * stmtName,
     short statsReqType,
     short statsMergeType,
     short activeQueryNum,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::GET_STATISTICS_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       stmtName_(stmtName),
       statsReqType_(statsReqType),
       statsMergeType_(statsMergeType),
       activeQueryNum_(activeQueryNum)
{
  setNodeType(ComTdb::ex_GET_STATISTICS);
}

Long ComTdbExeUtilGetStatistics::pack(void * space)
{
  if (stmtName_) 
    stmtName_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilGetStatistics::unpack(void * base, void * reallocator)
{
  if(stmtName_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilGetStatistics::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetStatistics :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if ( stmtName_ != (NABasicPtr)NULL )
	{
	  str_sprintf(buf,"StmtName = %s ", getStmtName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

void ComTdbExeUtilGetReorgStatistics::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetReorgStatistics :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if ( stmtName_ != (NABasicPtr)NULL )
	{
	  str_sprintf(buf,"Qid = %s ", getQid());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

void ComTdbExeUtilGetProcessStatistics::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetProcessStatistics :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if ( stmtName_ != (NABasicPtr)NULL )
	{
	  str_sprintf(buf,"Pid = %s ", getPid());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

//LCOV_EXCL_START
///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetUID
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetUID::ComTdbExeUtilGetUID
(
     Int64 uid,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::GET_UID_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       uid_(uid)
{
  setNodeType(ComTdb::ex_GET_UID);
}

Long ComTdbExeUtilGetUID::pack(void * space)
{
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilGetUID::unpack(void * base, void * reallocator)
{
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilGetUID::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[100];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetUID :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      str_sprintf(buf,"UID = %LD", uid_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilPopulateInMemStats
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilPopulateInMemStats::ComTdbExeUtilPopulateInMemStats
(
     Int64 uid,
     char * inMemHistogramsTableName,
     char * inMemHistintsTableName,
     char * sourceTableCatName,
     char * sourceTableSchName,
     char * sourceTableObjName,
     char * sourceHistogramsTableName,
     char * sourceHistintsTableName,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::POP_IN_MEM_STATS_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       uid_(uid),
       inMemHistogramsTableName_(inMemHistogramsTableName),
       inMemHistintsTableName_(inMemHistintsTableName),
       sourceTableCatName_(sourceTableCatName),
       sourceTableSchName_(sourceTableSchName),
       sourceTableObjName_(sourceTableObjName),
       sourceHistogramsTableName_(sourceHistogramsTableName),
       sourceHistintsTableName_(sourceHistintsTableName)
{
  setNodeType(ComTdb::ex_POP_IN_MEM_STATS);
}

Long ComTdbExeUtilPopulateInMemStats::pack(void * space)
{
  if (inMemHistogramsTableName_)
    inMemHistogramsTableName_.pack(space);
  if (inMemHistintsTableName_)
    inMemHistintsTableName_.pack(space);
  if (sourceTableCatName_)
    sourceTableCatName_.pack(space);
  if (sourceTableSchName_)
    sourceTableSchName_.pack(space);
  if (sourceTableObjName_)
    sourceTableObjName_.pack(space);
  if (sourceHistogramsTableName_)
    sourceHistogramsTableName_.pack(space);
  if (sourceHistintsTableName_)
    sourceHistintsTableName_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilPopulateInMemStats::unpack(void * base, void * reallocator)
{
  if (inMemHistogramsTableName_.unpack(base))
    return -1;
  if (inMemHistintsTableName_.unpack(base))
    return -1;
  if (sourceTableCatName_.unpack(base))
    return -1;
  if (sourceTableSchName_.unpack(base))
    return -1;
  if (sourceTableObjName_.unpack(base))
    return -1;
  if (sourceHistogramsTableName_.unpack(base))
    return -1;
  if (sourceHistintsTableName_.unpack(base))
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilPopulateInMemStats::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilPopulateInMemStats :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      str_sprintf(buf,"UID = %LD", uid_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

      if ((char *)inMemHistogramsTableName_ != (char *)NULL)
	{
	  str_sprintf(buf,"inMemHistogramsTableName_ = %s ", getInMemHistogramsTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      if ((char *)inMemHistintsTableName_ != (char *)NULL)
	{
	  str_sprintf(buf,"inMemHistintsTableName_ = %s ", getInMemHistintsTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if ((char *)sourceTableCatName_ != (char *)NULL)
	{
	  str_sprintf(buf,"sourceTableCatName_ = %s ", getSourceTableCatName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      if ((char *)sourceTableSchName_ != (char *)NULL)
	{
	  str_sprintf(buf,"sourceTableSchName_ = %s ", getSourceTableSchName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      if ((char *)sourceTableObjName_ != (char *)NULL)
	{
	  str_sprintf(buf,"sourceTableCatName_ = %s ", getSourceTableObjName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      if ((char *)sourceHistogramsTableName_ != (char *)NULL)
	{
	  str_sprintf(buf,"sourceHistogramsTableName_ = %s ", getSourceHistogramsTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      if ((char *)sourceHistintsTableName_ != (char *)NULL)
	{
	  str_sprintf(buf,"sourceHistintsTableName_ = %s ", getSourceHistintsTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
	
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}
//LCOV_EXCL_STOP
///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilUserLoad
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilUserLoad::ComTdbExeUtilUserLoad(
     char * tableName,
     ULng32 tableNameLen,
     ex_expr * input_expr,
     ULng32 input_rowlen,
     ex_expr * rwrsInputSizeExpr,
     ex_expr * rwrsMaxInputRowlenExpr,
     ex_expr * rwrsBufferAddrExpr,
     char * excpTabInsertStmt,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     const unsigned short childTuppIndex,
     const unsigned short excp_row_atp_index,
     ex_expr * excp_row_expr,
     ULng32 excp_row_len,
     Lng32 excpRowsPercentage,
     Lng32 excpRowsNumber,
     ULng32 num_table_cols,
     Int64 loadId,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::USER_LOAD_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     tableName, tableNameLen,
		     input_expr, input_rowlen,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       rwrsInputSizeExpr_(rwrsInputSizeExpr),
       rwrsMaxInputRowlenExpr_(rwrsMaxInputRowlenExpr),
       rwrsBufferAddrExpr_(rwrsBufferAddrExpr),
       excpTabInsertStmt_(excpTabInsertStmt),
       flags_(0),
       childTuppIndex_(childTuppIndex),
       excpRowAtpIndex_(excp_row_atp_index),
       excpRowExpr_(excp_row_expr),
       excpRowLen_(excp_row_len),
       excpRowsPercentage_(excpRowsPercentage),
       excpRowsNumber_(excpRowsNumber),
       numTableCols_(num_table_cols),
       loadId_(loadId)
{
  setNodeType(ComTdb::ex_USER_LOAD);
}

Long ComTdbExeUtilUserLoad::pack(void * space)
{
  rwrsInputSizeExpr_.pack(space);
  rwrsMaxInputRowlenExpr_.pack(space);
  rwrsBufferAddrExpr_.pack(space);

  if (excpTabInsertStmt_) 
    excpTabInsertStmt_.pack(space);

  excpRowExpr_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilUserLoad::unpack(void * base, void * reallocator)
{
  if (rwrsInputSizeExpr_.unpack(base, reallocator)) return -1;
  if (rwrsMaxInputRowlenExpr_.unpack(base, reallocator)) return -1;
  if (rwrsBufferAddrExpr_.unpack(base, reallocator)) return -1;

  if (excpTabInsertStmt_.unpack(base)) 
    return -1;

  if (excpRowExpr_.unpack(base, reallocator)) return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilUserLoad::displayContents(Space * space,
					      ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilUserLoad :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));
	}

      if (excpTabInsertStmt_)
	{
	  char query[400];
	  if (strlen(excpTabInsertStmt_) > 390)
	    {
	      strncpy(query, excpTabInsertStmt_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, excpTabInsertStmt_);

	  str_sprintf(buf,"Exception Table Insert Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilSidetreeInsert
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilSidetreeInsert::ComTdbExeUtilSidetreeInsert(
     char * tableName,
     ULng32 tableNameLen,
     char * originalQuery,
     char * sidetreeQuery,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::SIDETREE_INSERT_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     tableName, tableNameLen,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       originalQuery_(originalQuery),
       sidetreeQuery_(sidetreeQuery),
       flags_(0)
{
  setNodeType(ComTdb::ex_SIDETREE_INSERT);
}

Long ComTdbExeUtilSidetreeInsert::pack(void * space)
{
  if (originalQuery_) 
    originalQuery_.pack(space);
  if (sidetreeQuery_) 
    sidetreeQuery_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilSidetreeInsert::unpack(void * base, void * reallocator)
{
  if (originalQuery_.unpack(base)) 
    return -1;
  if (sidetreeQuery_.unpack(base)) 
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilSidetreeInsert::displayContents(Space * space,
					      ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilSidetreeInsert :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));
	}

      if (originalQuery_)
	{
	  char query[400];
	  if (strlen(originalQuery_) > 390)
	    {
	      strncpy(query, originalQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, originalQuery_);

	  str_sprintf(buf,"Original Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (sidetreeQuery_)
	{
	  char query[400];
	  if (strlen(originalQuery_) > 390)
	    {
	      strncpy(query, sidetreeQuery_, 390);
	      query[390] = 0;
	      strcat(query, "...");
	    }
	  else
	    strcpy(query, sidetreeQuery_);

	  str_sprintf(buf,"Sidetree Query = %s ",query);
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilAqrWnrInsert
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilAqrWnrInsert::ComTdbExeUtilAqrWnrInsert(
     char * tableName,
     ULng32 tableNameLen,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::AQR_WNR_INSERT_,
		     (char *) eye_AQR_WNR_INS, 
                     0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     tableName, tableNameLen,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       aqrWnrInsflags_(0)
{
  setNodeType(ComTdb::ex_ARQ_WNR_INSERT);
}

void ComTdbExeUtilAqrWnrInsert::displayContents(Space * space,
					      ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilAqrWnrInsert:");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

      if (getTableName() != NULL)
        {
          str_sprintf(buf,"Tablename = %s ",getTableName());
          space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					           sizeof(short));
        }
      str_sprintf(buf, "Lock target = %s ",  doLockTarget() ? "ON" : "OFF");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));

    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}
//////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilLongRunning
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilLongRunning::ComTdbExeUtilLongRunning(
     char * tableName,
     ULng32 tableNameLen,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::LONG_RUNNING_,
 		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     tableName, tableNameLen,
  		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0),
       lruStmt_(NULL),
       lruStmtLen_(0),
       lruStmtWithCK_(NULL),
       lruStmtWithCKLen_(0),
       predicate_(NULL),
       predicateLen_(0),
       multiCommitSize_(0)
{
  setNodeType(ComTdb::ex_LONG_RUNNING);
}

Long ComTdbExeUtilLongRunning::pack(void * space)
{
  if (lruStmt_)
    lruStmt_.pack(space);

  if(lruStmtWithCK_)
    lruStmtWithCK_.pack(space);

  if (predicate_)
    predicate_.pack(space); 

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilLongRunning::unpack(void * base, void * reallocator)
{

  if(lruStmt_.unpack(base))
    return -1;
  
  if(lruStmtWithCK_.unpack(base))
    return -1;

  if (predicate_.unpack(base))
     return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilLongRunning::setPredicate(Space *space, char *predicate)
{
   if (predicate != NULL)
   {
      predicateLen_ = strlen(predicate);
      predicate_ = space->allocateAlignedSpace
        ((ULng32)predicateLen_ + 1);
      strcpy(predicate_, predicate);
   }
}

void ComTdbExeUtilLongRunning::displayContents(Space * space,
					      ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilLongRunning :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (getTableName() != NULL)
	{
	  str_sprintf(buf,"Tablename = %s ",getTableName());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), 
					       sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
    }
}




///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetMetadataInfo
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetMetadataInfo::ComTdbExeUtilGetMetadataInfo
(
     QueryType queryType,
     char *    cat,
     char *    sch,
     char *    obj,
     char *    pattern,
     char *    param1,
     ex_expr_base * scan_expr,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::GET_METADATA_INFO_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     scan_expr,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       queryType_(queryType),
       cat_(cat), sch_(sch), obj_(obj),
       pattern_(pattern),
       param1_(param1),
       flags_(0)
{
  setNodeType(ComTdb::ex_GET_METADATA_INFO);
}

Long ComTdbExeUtilGetMetadataInfo::pack(void * space)
{
  if (cat_) 
    cat_.pack(space);
  if (sch_) 
    sch_.pack(space);
  if (obj_) 
    obj_.pack(space);
  if (pattern_) 
    pattern_.pack(space);
  if (param1_) 
    param1_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilGetMetadataInfo::unpack(void * base, void * reallocator)
{
  if (cat_.unpack(base))
    return -1;
  if (sch_.unpack(base))
    return -1;
  if (obj_.unpack(base))
    return -1;
  if (pattern_.unpack(base))
    return -1;
  if (param1_.unpack(base))
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilGetMetadataInfo::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilGetMetadataInfo :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      str_sprintf(buf, "QueryType: %d", queryType_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

      if (getCat() != NULL)
	{
	  str_sprintf(buf,"Catalog = %s ", getCat());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (getSch() != NULL)
	{
	  str_sprintf(buf,"Schema = %s ", getSch());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (getObj() != NULL)
	{
	  str_sprintf(buf,"Object = %s ", getObj());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (getPattern() != NULL)
	{
	  str_sprintf(buf,"Pattern = %s ", getPattern());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      if (getParam1() != NULL)
	{
	  str_sprintf(buf,"Param1 = %s ", getParam1());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}

      str_sprintf(buf, "Flags = %b",flags_);
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetHiveMetadataInfo
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetHiveMetadataInfo::ComTdbExeUtilGetHiveMetadataInfo
(
     QueryType queryType,
     char *    cat,
     char *    sch,
     char *    obj,
     char *    pattern,
     char *    param1,
     ex_expr_base * scan_expr,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtilGetMetadataInfo(
				    queryType,
				    cat, sch, obj, pattern, param1,
				    scan_expr,
				    work_cri_desc, work_atp_index,
				    given_cri_desc, returned_cri_desc,
				    down, up, 
				    num_buffers, buffer_size),
       unused1_(NULL),
       unused2_(NULL),
       unused3_(NULL),
       unused4_(NULL)
{
  setType(ComTdbExeUtil::GET_HIVE_METADATA_INFO_);
  setNodeType(ComTdb::ex_GET_HIVE_METADATA_INFO);
}

Long ComTdbExeUtilGetHiveMetadataInfo::pack(void * space)
{
  return ComTdbExeUtilGetMetadataInfo::pack(space);
}

Lng32 ComTdbExeUtilGetHiveMetadataInfo::unpack(void * base, void * reallocator)
{
  return ComTdbExeUtilGetMetadataInfo::unpack(base, reallocator);
}

void ComTdbExeUtilGetHiveMetadataInfo::displayContents(Space * space,ULng32 flag)
{
  ComTdbExeUtilGetMetadataInfo::displayContents(space,flag & 0xFFFFFFFE);
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


// On Linux, suspend/activate uses ComTdbCancel
///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilGetDiskLabelStats
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilGetDiskLabelStats::ComTdbExeUtilGetDiskLabelStats
(
     ex_expr_base * input_expr,
     ULng32 input_rowlen,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::GET_DISK_LABEL_STATS_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     input_expr, input_rowlen,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       flags_(0)
{
  setNodeType(ComTdb::ex_GET_DISK_LABEL_STATS);
}

Long ComTdbExeUtilGetDiskLabelStats::pack(void * space)
{
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilGetDiskLabelStats::unpack(void * base, void * reallocator)
{
  return ComTdbExeUtil::unpack(base, reallocator);
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilShowSet
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilShowSet::ComTdbExeUtilShowSet
(
     UInt16 type,
     char * param1,
     char * param2,
     ex_cri_desc * work_cri_desc,
     const unsigned short work_atp_index,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::SHOW_SET_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     work_cri_desc, work_atp_index,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       type_(type), flags_(0),
       param1_(param1), param2_(param2)
{
  setNodeType(ComTdb::ex_SHOW_SET);
}

Long ComTdbExeUtilShowSet::pack(void * space)
{
  if (param1_) 
    param1_.pack(space);
  if (param2_) 
    param2_.pack(space);
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilShowSet::unpack(void * base, void * reallocator)
{
  if(param1_.unpack(base))
    return -1;
  if(param2_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilShowSet::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[100];
      str_sprintf(buf, "\nFor ComTdbExeUtilShowSet :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilAQR
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilAQR::ComTdbExeUtilAQR
(
     Lng32 task,
     ex_cri_desc * given_cri_desc,
     ex_cri_desc * returned_cri_desc,
     queue_index down,
     queue_index up,
     Lng32 num_buffers,
     ULng32 buffer_size)
     : ComTdbExeUtil(ComTdbExeUtil::AQR_,
		     NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		     NULL, 0,
		     NULL, 0,
		     NULL, 0,
		     NULL,
		     NULL, 0,
		     given_cri_desc, returned_cri_desc,
		     down, up, 
		     num_buffers, buffer_size),
       task_(task),
       flags_(0)
{
  setNodeType(ComTdb::ex_AQR);
}

Long ComTdbExeUtilAQR::pack(void * space)
{
  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilAQR::unpack(void * base, void * reallocator)
{
  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilAQR::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[100];
      str_sprintf(buf, "\nFor ComTdbExeUtilAQR :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilLobExtract
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilLobExtract::ComTdbExeUtilLobExtract
(
 char * handle,
 Lng32 handleLen,
 ExtractToType toType,
 Int64 size,
 Int64 size2,
 Lng32 lobStorageType,
 char * stringParam1,
 char * stringParam2,
 char * stringParam3,
 char * lobHdfsServer,
 Lng32 lobHdfsPort,
  ex_expr * input_expr,
 ULng32 input_rowlen,
 ex_cri_desc * work_cri_desc,
 const unsigned short work_atp_index,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
  : ComTdbExeUtil(ComTdbExeUtil::LOB_EXTRACT_,
		  NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		  NULL, 0,
		  input_expr, input_rowlen,
		  NULL, 0,
		  NULL,
		  work_cri_desc, work_atp_index,
		  given_cri_desc, returned_cri_desc,
		  down, up, 
		  num_buffers, buffer_size),
    handle_(handle),
    handleLen_(handleLen),
    toType_((short)toType),
    size_(size),
    size2_(size2),
    lobStorageType_(lobStorageType),
    stringParam1_(stringParam1),
    stringParam2_(stringParam2),
    stringParam3_(stringParam3),
    lobHdfsServer_(lobHdfsServer),
    lobHdfsPort_(lobHdfsPort),
    flags_(0)
{
  setNodeType(ComTdb::ex_LOB_EXTRACT);
}

Long ComTdbExeUtilLobExtract::pack(void * space)
{
  if (handle_)
    handle_.pack(space);

  if (stringParam1_)
    stringParam1_.pack(space);
  
  if (stringParam2_)
    stringParam2_.pack(space);
  
  if (stringParam3_)
    stringParam3_.pack(space);
  
  if (lobHdfsServer_)
    lobHdfsServer_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilLobExtract::unpack(void * base, void * reallocator)
{
  if (handle_.unpack(base))
    return -1;

  if (stringParam1_.unpack(base))
    return -1;

  if (stringParam2_.unpack(base))
    return -1;

  if (stringParam3_.unpack(base))
    return -1;

  if (lobHdfsServer_.unpack(base))
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilLobExtract::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[100];
      str_sprintf(buf, "\nFor ComTdbExeUtilLobExtract :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilLobShowddl
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilLobShowddl::ComTdbExeUtilLobShowddl
(
 char * tableName,
 char * schName,
 short schNameLen,
 Int64 objectUID,
 Lng32 numLOBs,
 char * lobNumArray,
 char * lobLocArray,
 short maxLocLen,
 short sdOptions,
 ex_cri_desc * given_cri_desc,
 ex_cri_desc * returned_cri_desc,
 queue_index down,
 queue_index up,
 Lng32 num_buffers,
 ULng32 buffer_size)
  : ComTdbExeUtil(ComTdbExeUtil::LOB_SHOWDDL_,
		  NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
		  tableName, strlen(tableName),
		  NULL, 0,
		  NULL, 0,
		  NULL,
		  NULL, 0,
		  given_cri_desc, returned_cri_desc,
		  down, up, 
		  num_buffers, buffer_size),
    flags_(0),
    objectUID_(objectUID),
    numLOBs_(numLOBs),
    lobNumArray_(lobNumArray),
    lobLocArray_(lobLocArray),
    maxLocLen_(maxLocLen),
    sdOptions_(sdOptions),
    schName_(schName),
    schNameLen_(schNameLen)
{
  setNodeType(ComTdb::ex_LOB_SHOWDDL);
}

Long ComTdbExeUtilLobShowddl::pack(void * space)
{
  if (schName_) 
    schName_.pack(space);

 if (lobNumArray_) 
    lobNumArray_.pack(space);

 if (lobLocArray_) 
    lobLocArray_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilLobShowddl::unpack(void * base, void * reallocator)
{
  if(schName_.unpack(base))
    return -1;

  if(lobNumArray_.unpack(base)) 
    return -1;

  if(lobLocArray_.unpack(base)) 
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilLobShowddl::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[100];
      str_sprintf(buf, "\nFor ComTdbExeUtilLobShowddl :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


short ComTdbExeUtilLobShowddl::getLOBnum(short i)
{
  if ((i > numLOBs_) || (i <= 0))
    return -1;

  short lobNum = *((short*)&getLOBnumArray()[2*(i-1)]);

  return lobNum;
}

char * ComTdbExeUtilLobShowddl::getLOBloc(short i)
{
  if ((i > numLOBs_) || (i <= 0))
    return NULL;

  char * lobLoc = &getLOBlocArray()[maxLocLen_*(i-1)];

  return lobLoc;
}
/////////////////////////////////////////////////////////////////////////////////
// class ComTdbExeUtilHiveMDaccess
/////////////////////////////////////////////////////////////////////////////////
ComTdbExeUtilHiveMDaccess::ComTdbExeUtilHiveMDaccess() 
  : ComTdbExeUtil()
{
}

ComTdbExeUtilHiveMDaccess::ComTdbExeUtilHiveMDaccess(
			   MDType type,
			   ULng32 tupleLen,
			   ex_cri_desc *criDescParentDown,
			   ex_cri_desc *criDescParentUp,
			   ex_cri_desc *workCriDesc,
			   unsigned short workAtpIndex,
			   queue_index queueSizeDown,
			   queue_index queueSizeUp,
			   Lng32 numBuffers,
			   ULng32 bufferSize,
			   ex_expr *scanPred,
			   char * hivePredStr,
                           char * schemaName)
  : ComTdbExeUtil(ComTdbExeUtil::HIVE_MD_ACCESS_,
		  0, 0, 0, // query,querylen,querycharset
		  NULL, 0, // tablename,tablenamelen
		  NULL, tupleLen, 
		  NULL, tupleLen,
		  scanPred,
		  workCriDesc, workAtpIndex,
		  criDescParentDown, criDescParentUp,
		  queueSizeDown, queueSizeUp,
		  numBuffers, bufferSize),
    mdType_(type),
    hivePredStr_(hivePredStr),
    schema_(schemaName)
{
  setNodeType(ComTdb::ex_HIVE_MD_ACCESS);
}

// Return the number of expressions held by the explain TDB (2)
// They are enumerated as: 0 - scanPred, 1 - paramsExpr
Int32
ComTdbExeUtilHiveMDaccess::numExpressions() const
{
  return(1);
}
 
// Return the expression names of the explain TDB based on some 
// enumeration. 0 - scanPred, 1 - paramsExpr
const char *
ComTdbExeUtilHiveMDaccess::getExpressionName(Int32 expNum) const
{
  switch(expNum)
    {
    case 0:
      return "Scan Expr";
    default:
      return 0;
    }  
}

// Return the expressions of the explain TDB based on some 
// enumeration. 0 - scanPred, 1 - paramsExpr
ex_expr *
ComTdbExeUtilHiveMDaccess::getExpressionNode(Int32 expNum)
{
  switch(expNum)
    {
    case 0:
      return scanExpr_;
    default:
      return 0;
    }  
}

// Pack the explainTdb: Convert all pointers to offsets relative
// to the space object.
Long ComTdbExeUtilHiveMDaccess::pack(void * space)
{
  if (hivePredStr_) 
    hivePredStr_.pack(space);
  if (schema_)
    schema_.pack(space);

  return ComTdbExeUtil::pack(space);
}

// Unpack the explainTdb.: Convert all offsets relative to base
// to pointers
Lng32 ComTdbExeUtilHiveMDaccess::unpack(void * base, void * reallocator)
{
  if (hivePredStr_.unpack(base))
    return -1;
  if (schema_.unpack(base))
    return -1;

  return ComTdbExeUtil::unpack(base, reallocator);
}

void ComTdbExeUtilHiveMDaccess::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);
  
  if(flag & 0x00000008)
    {
      char buf[2000];
      str_sprintf(buf, "\nFor ComTdbExeUtilHiveMDaccess :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
      
      if (hivePredStr())
	{
	  str_sprintf(buf,"hivePredStr_ = %s", hivePredStr());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
      if (getSchema())
	{
	  str_sprintf(buf,"schema_ = %s", getSchema());
	  space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
	}
    }
  
  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Methods for class ComTdbExeUtilMetadataUpgrade
//
///////////////////////////////////////////////////////////////////////////
ComTdbExeUtilMetadataUpgrade::ComTdbExeUtilMetadataUpgrade(
					     ex_expr * output_expr,
					     ULng32 output_rowlen,
					     ex_cri_desc * workCriDesc,
					     const unsigned short work_atp_index,
					     ex_cri_desc *criDescParentDown,
					     ex_cri_desc *criDescParentUp,
					     queue_index queueSizeDown,
					     queue_index queueSizeUp,
					     Lng32 numBuffers,
					     ULng32 bufferSize)
  : ComTdbExeUtil(ComTdbExeUtil::UPGRADE_MD_,
		  0, 0, 0, // query,querylen,querycharset
		  NULL, 0, // tablename,tablenamelen
		  NULL, 0,
		  NULL, 0,
		  NULL,
		  workCriDesc, work_atp_index,
		  criDescParentDown,
		  criDescParentUp,
		  queueSizeDown,
		  queueSizeUp,
		  numBuffers,  
		  bufferSize),
    flags_(0)
{
  setNodeType(ex_METADATA_UPGRADE);
  setEyeCatcher(eye_METADATA_UPGRADE);
}


//*********************************************
//ComTdbExeUtilHBaseBulkLoad
//********************************************
ComTdbExeUtilHBaseBulkLoad::ComTdbExeUtilHBaseBulkLoad(char * tableName,
                           ULng32 tableNameLen,
                           char * ldStmtStr,
                           ex_cri_desc * work_cri_desc,
                           const unsigned short work_atp_index,
                           ex_cri_desc * given_cri_desc,
                           ex_cri_desc * returned_cri_desc,
                           queue_index down,
                           queue_index up,
                           Lng32 num_buffers,
                           ULng32 buffer_size
                           )
    : ComTdbExeUtil(ComTdbExeUtil::HBASE_LOAD_,
                    NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
                    tableName, tableNameLen,
                    NULL, 0,
                    NULL, 0,
                    NULL,
                    work_cri_desc, work_atp_index,
                    given_cri_desc, returned_cri_desc,
                    down, up,
                    num_buffers, buffer_size),
      ldQuery_(ldStmtStr),
      flags_(0)
    {
    setNodeType(ComTdb::ex_HBASE_LOAD);
    }

Long ComTdbExeUtilHBaseBulkLoad::pack(void * space)
{
  if (ldQuery_)
    ldQuery_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilHBaseBulkLoad::unpack(void * base, void * reallocator)
{
  if(ldQuery_.unpack(base))
    return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}
void ComTdbExeUtilHBaseBulkLoad::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);

  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilHbaseLoad :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

      if (getTableName() != NULL)
        {
          str_sprintf(buf,"Tablename = %s ",getTableName());
          space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
        }

      if (ldQuery_)
        {
          char query[400];
          if (strlen(ldQuery_) > 390)
            {
              strncpy(query, ldQuery_, 390);
              query[390] = 0;
              strcat(query, "...");
            }
          else
            strcpy(query, ldQuery_);

          str_sprintf(buf,"ld Query = %s ",query);
          space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
        }


    }

  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}


//*********************************************
//ComTdbExeUtilHBaseBulkUnLoad
//********************************************
ComTdbExeUtilHBaseBulkUnLoad::ComTdbExeUtilHBaseBulkUnLoad(char * tableName,
                           ULng32 tableNameLen,
                           char * uldStmtStr,
                           char * extractLocation,
                           ex_cri_desc * work_cri_desc,
                           const unsigned short work_atp_index,
                           ex_cri_desc * given_cri_desc,
                           ex_cri_desc * returned_cri_desc,
                           queue_index down,
                           queue_index up,
                           Lng32 num_buffers,
                           ULng32 buffer_size
                           )
    : ComTdbExeUtil(ComTdbExeUtil::HBASE_UNLOAD_,
                    NULL, 0, (Int16)SQLCHARSETCODE_UNKNOWN,
                    tableName, tableNameLen,
                    NULL, 0,
                    NULL, 0,
                    NULL,
                    work_cri_desc, work_atp_index,
                    given_cri_desc, returned_cri_desc,
                    down, up,
                    num_buffers, buffer_size),
      uldQuery_(uldStmtStr),
      flags_(0),
      compressType_(0),
      extractLocation_(extractLocation)
    {
    setNodeType(ComTdb::ex_HBASE_UNLOAD);
    }

Long ComTdbExeUtilHBaseBulkUnLoad::pack(void * space)
{
  if (uldQuery_)
    uldQuery_.pack(space);

  if (mergePath_)
    mergePath_.pack(space);
  if (extractLocation_)
    extractLocation_.pack(space);

  return ComTdbExeUtil::pack(space);
}

Lng32 ComTdbExeUtilHBaseBulkUnLoad::unpack(void * base, void * reallocator)
{
  if(uldQuery_.unpack(base))
    return -1;
  if(mergePath_.unpack(base))
    return -1;
  if(extractLocation_.unpack(base))
      return -1;
  return ComTdbExeUtil::unpack(base, reallocator);
}
void ComTdbExeUtilHBaseBulkUnLoad::displayContents(Space * space,ULng32 flag)
{
  ComTdb::displayContents(space,flag & 0xFFFFFFFE);

  if(flag & 0x00000008)
    {
      char buf[1000];
      str_sprintf(buf, "\nFor ComTdbExeUtilHbaseUnLoad :");
      space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));

      if (getTableName() != NULL)
        {
          str_sprintf(buf,"Tablename = %s ",getTableName());
          space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
        }

      if (uldQuery_)
        {
          char query[400];
          if (strlen(uldQuery_) > 390)
            {
              strncpy(query, uldQuery_, 390);
              query[390] = 0;
              strcat(query, "...");
            }
          else
            strcpy(query, uldQuery_);

          str_sprintf(buf,"uld Query = %s ",query);
          space->allocateAndCopyToAlignedSpace(buf, str_len(buf), sizeof(short));
        }


    }

  if (flag & 0x00000001)
    {
      displayExpression(space,flag);
      displayChildren(space,flag);
    }
}
