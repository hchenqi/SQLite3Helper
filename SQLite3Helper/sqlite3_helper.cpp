#include "sqlite3_helper.h"

#include <winsqlite/winsqlite3.h>


#pragma comment(lib, "winsqlite3.lib")



namespace SQLite3Helper {

namespace {

constexpr size_t max_blob_length = 4096;  // 4kb

int index = 0;

constexpr struct Result {
	const Result& operator<<(int res) const {
		if (res != SQLITE_OK) { throw std::runtime_error("sqlite error: " + std::to_string(res)); }
		return *this;
	}
}res;

inline sqlite3* AsSqliteDb(void* db) { return static_cast<sqlite3*>(db); }
inline sqlite3** AsSqliteDb(void** db) { return reinterpret_cast<sqlite3**>(db); }

inline sqlite3_stmt* AsSqliteStmt(void* stmt) { return static_cast<sqlite3_stmt*>(stmt); }
inline sqlite3_stmt** AsSqliteStmt(void** stmt) { return reinterpret_cast<sqlite3_stmt**>(stmt); }

}


Query::~Query() {
	sqlite3_finalize(AsSqliteStmt(command));
}


Database::Database(const char file[]) : db(nullptr) {
	res << sqlite3_open(file, AsSqliteDb(&db));
	PrepareQuery(begin_transaction);
	PrepareQuery(commit);
	PrepareQuery(rollback);
}

Database::~Database() {
	res << sqlite3_close_v2(AsSqliteDb(db));
}

void Database::PrepareQuery(Query& query) {
	if (query.command == nullptr) {
		res << sqlite3_prepare_v2(AsSqliteDb(db), query.c_str(), (int)query.length(), AsSqliteStmt(&query.command), nullptr);
	}
	res << sqlite3_reset(AsSqliteStmt(query.command));
	index = 1;
}

bool Database::ExecuteQuery(Query& query) {
	int ret = sqlite3_step(AsSqliteStmt(query.command));
	index = 0;
	if (ret == SQLITE_ROW) { return true; }
	if (ret == SQLITE_DONE) { return false; }
	res << ret;
}

uint64 Database::Changes() {
	return sqlite3_changes64(AsSqliteDb(db));
}

void Database::Bind(Query& query, uint64 object) {
	res << sqlite3_bind_int64(AsSqliteStmt(query.command), index++, object);
}

void Database::Bind(Query& query, const void* data, size_t length) {
	if (length > max_blob_length) { throw std::runtime_error("blob too large"); }
	res << sqlite3_bind_blob(AsSqliteStmt(query.command), index++, data, (int)length, SQLITE_STATIC);
}

void Database::Read(Query& query, uint64& object) {
	object = sqlite3_column_int64(AsSqliteStmt(query.command), index++);
}

void Database::Read(Query& query, const void*& data, size_t& length) {
	data = sqlite3_column_blob(AsSqliteStmt(query.command), index);
	length = sqlite3_column_bytes(AsSqliteStmt(query.command), index++);
}

} // namespace SQLite3Helper
