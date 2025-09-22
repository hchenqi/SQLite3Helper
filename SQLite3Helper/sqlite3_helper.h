#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include <stdexcept>


namespace SQLite3Helper {

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
	void Bind(Query& query, const void* data, size_t length);
private:
	template<class T> requires std::is_trivial_v<T>
	void Bind(Query& query, const std::basic_string<T>& object) {
		Bind(query, object.data(), object.size() * sizeof(T));
	}
	template<class T> requires std::is_trivial_v<T>
	void Bind(Query& query, const std::vector<T>& object) {
		Bind(query, object.data(), object.size() * sizeof(T));
	}
	template<class T1, class T2>
	void Bind(Query& query, const std::pair<T1, T2>& object) {
		Bind(query, object.first); Bind(query, object.second);
	}
	template<class... Ts>
	void Bind(Query& query, const std::tuple<Ts...>& object) {
		std::apply([&](auto&... member) { (Bind(query, member), ...); }, object);
	}

private:
	void Read(Query& query, uint64& object);
	void Read(Query& query, const void*& data, size_t& length);
private:
	template<class T> requires std::is_trivial_v<T>
	void Read(Query& query, std::basic_string<T>& object) {
		const void* data; size_t length; Read(query, data, length);
		if (length % sizeof(T)) { throw std::runtime_error("length error"); }
		const T* begin = static_cast<const T*>(data); length /= sizeof(T);
		object.assign(begin, length);
	}
	template<class T> requires std::is_trivial_v<T>
	void Read(Query& query, std::vector<T>& object) {
		const void* data; size_t length; Read(query, data, length);
		if (length % sizeof(T)) { throw std::runtime_error("length error"); }
		const T* begin = static_cast<const T*>(data); length /= sizeof(T);
		object.assign(begin, begin + length);
	}
	template<class T1, class T2>
	void Read(Query& query, std::pair<T1, T2>& object) {
		Read(query, object.first); Read(query, object.second);
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

} // namespace SQLite3Helper
