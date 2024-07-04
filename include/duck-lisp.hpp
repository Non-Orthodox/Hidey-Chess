#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
extern "C" {
#include "duck-lisp/DuckLib/memory.h"
#include "duck-lisp/duckLisp.h"
}

class DuckLisp {
private:
	std::map<std::string, void *> sharedUserData;
public:
	duckLisp_t duckLisp;
	dl_memoryAllocation_t memoryAllocation;
	DuckLisp(const std::size_t);
	~DuckLisp();
	dl_error_t print_errors();
	dl_error_t registerParserAction(const std::string name,
	                                dl_error_t (*callback)(duckLisp_t*, duckLisp_ast_compoundExpression_t*));
	int registerGenerator();
	int registerCallback(const std::string name, const std::string typeString, dl_error_t (*callback)(duckVM_t *));
	void *getUserDataByName(const std::string name);
	void setUserDataByName(const std::string name, void *value);
};

class DuckVM {
private:
	std::map<std::string, void *> sharedUserData;
public:
	duckVM_t duckVM;
	dl_memoryAllocation_t memoryAllocation;
	std::string instanceName;
	DuckVM(const std::string, const std::size_t, const std::size_t);
	~DuckVM();
	dl_error_t print_errors();
	int registerCallback(std::ptrdiff_t index, dl_error_t (*callback)(duckVM_t *));
	dl_error_t garbageCollect();
	void *getUserDataByName(const std::string name);
	void setUserDataByName(const std::string name, void *value);
};

int registerCallback(std::shared_ptr<DuckVM> duckVM,
                     std::shared_ptr<DuckLisp> duckLisp,
                     const std::string name,
                     const std::string typeString,
                     dl_error_t (*callback)(duckVM_t *));
int eval(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp, const std::string source);
// int funcall(std::shared_ptr<DuckVM> duckVM,
//             std::shared_ptr<DuckLisp> duckLisp,
//             const std::string name,
//             const std::size_t numberOfArgs);

void *getUserDataByName(duckVM_t *duckVM, const std::string name);
void *getUserDataByName(duckLisp_t *duckLisp, const std::string name);
void setUserDataByName(duckVM_t *duckVM, const std::string name, void *value);
void setUserDataByName(duckLisp_t *duckLisp, const std::string name, void *value);
