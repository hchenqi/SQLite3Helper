#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include <stdexcept>


namespace SqliteHelper {

using byte = std::byte;
using uint64 = unsigned long long;


class Query : std::string {
public:
	using std::string::string;
	~Query();
private:
	Query(const Query&) = delete;
	Query& operator=(const Query&) = delete;
private:
	friend class Database;
	void* command = nullptr;
};


class Database {
public:
	Database(const char file[]);
	~Database();
private:
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;
private:
	void* db;
private:
	void PrepareQuery(Query& query);
	bool ExecuteQuery(Query& query);
private:
	void Bind(Query& query, uint64 object);
	void Bind(Query& query, const std::string& object);
	void Bind(Query& query, const std::vector<byte>& object);
private:
	void Read(Query& query, uint64& object);
	void Read(Query& query, std::string& object);
	void Read(Query& query, std::vector<byte>& object);
private:
	template<class... Ts>
	void Bind(Query& query, const std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (Bind(query, member), ...); }, object);
	}
	template<class... Ts>
	void Read(Query& query, std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (Read(query, member), ...); }, object);
	}
public:
	template<class... Ts>
	void Execute(Query& query, const Ts&... para) {
		PrepareQuery(query);
		(Bind(query, para), ...);
		ExecuteQuery(query);
	}
	template<class T, class... Ts>
	T ExecuteForOne(Query& query, const Ts&... para) {
		PrepareQuery(query);
		(Bind(query, para), ...);
		if (ExecuteQuery(query) == false) { throw std::runtime_error("sqlite error"); }
		T result;
		Read(query, result);
		return result;
	}
	template<class T, class... Ts>
	std::vector<T> ExecuteForMultiple(Query& query, const Ts&... para) {
		PrepareQuery(query);
		(Bind(query, para), ...);
		std::vector<T> result;
		while (ExecuteQuery(query)) { Read(query, result.emplace_back()); }
		return result;
	}
};


}