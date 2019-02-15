#pragma once
#ifndef __H_SQL_WARPPER_H__
#define __H_SQL_WARPPER_H__

#include <iostream>
#include <string>
#include "log_writer.h"
#include "sql_operate_ipml.h"

class sql_warpper : public sql_operate_ipml
{
public:
	sql_warpper(const std::string & host, const std::string & user, 
		const std::string & pwd, const std::string & db) 
		: host_(host), user_(user), pwd_(pwd), db_(db)
	{
		/* Create a connection */
		driver_ = get_driver_instance();
		//con_ = driver_->connect("tcp://127.0.0.1:3306", "root", "407043");
		con_ = driver_->connect(host_.c_str(), user_.c_str(), pwd_.c_str());
		/* Connect to the MySQL test database */
		con_->setSchema(db_.c_str());

		stmt_ = con_->createStatement();
	}

	virtual ~sql_warpper() 
	{
		delete stmt_;
		delete con_;
	}

	// ����
	bool create(const std::string & command) { return invoke(command); }
	// ����
	bool insert(const std::string & command) { return invoke(command); }
	// ɾ��
	bool remove(const std::string & command) { return invoke(command); }
	// ���� 
	bool update(const std::string & command) { return invoke(command); }
	// ��ѯ
	template <typename __dest_type, typename __set, typename __type, typename ... params>
	bool select(const std::string & command, std::vector<__dest_type> & dest, __set parm)
	{
		try {
			/* Select in ascending order */
			pstmt_ = con_->prepareStatement(command.c_str());
			res_ = pstmt_->executeQuery();

			/* Fetch in reverse = descending order! */
			res_->afterLast();
			while (res_->previous())
			{
				separation<__dest_type, sql::ResultSet, __set, __type, params...>(dest, res_, parm);
			}
			delete res_;
			delete pstmt_;
		}
		catch (sql::SQLException &e) {
			std::string str_logger_("sql error by create select -> code is " + 
				std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
			return false;
		}
		return true;
	}

private:
	// ִ������
	bool invoke(const std::string & command)
	{
		try {
			//stmt_->execute("DROP TABLE IF EXISTS test");
			stmt_->execute(command.c_str());
		}
		catch (sql::SQLException &e) {
			std::string str_logger_("sql error by update command -> code is "
				+ std::to_string(e.getErrorCode()) + " & describe is " + std::string(e.what()));
			wstd::log_writer::log_store(str_logger_, __FILE_LINE__);
			return false;
		}
		return true;
	}

	std::string host_;
	std::string user_;
	std::string pwd_;
	std::string db_;

	sql::Driver * driver_;
	sql::Connection * con_;
	sql::Statement * stmt_;
	sql::ResultSet * res_;
	sql::PreparedStatement * pstmt_;
};

#endif

