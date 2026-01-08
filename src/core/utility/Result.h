#pragma once
#include <optional>

// Ç¢Ç¢ä¥Ç∂ÇÃÇ™å©Ç¬ÇØÇÈÇÃÇ™ñ ì|ÇæÇ¡ÇΩÇÃÇ≈é©çÏ

namespace inner {
	enum class ResultType : int {
		Ok,
		Err,
	};

	template <typename T, typename E>
	union ResultValue {
		T ok_value;
		E err_value;
	};
}

template <typename T, typename E>
class Result {
private:
	const inner::ResultType result_type;
	const inner::ResultValue<T, E> value;

	explicit Result(void) = delete;

public:
	explicit Result(const T&& ok_value) noexcept;
	explicit Result(const E&& err_value) noexcept;

	T&& unwrap(void);
	E&& unwrap_err(void);
	T&& unwrap_or(T or_value);

	std::optional<T> ok(void) noexcept;
	std::optional<E> err(void) noexcept;

	bool is_ok(void) noexcept;
	bool is_err(void) noexcept;
};

template<typename T, typename E>
inline Result<T, E>::Result(const T&& ok_value) noexcept :
	result_type(inner::ResultType::Ok),
	value(std::move(ok_value)) {
}

template<typename T, typename E>
inline Result<T, E>::Result(const E&& err_value) noexcept :
	result_type(inner::ResultType::Err),
	value(std::move(err_value)) {
}

template<typename T, typename E>
inline T&& Result<T, E>::unwrap(void) {
	if(this->is_err()) {
		throw;
	}
	return std::move(this->value.ok_value);
}

template<typename T, typename E>
inline E&& Result<T, E>::unwrap_err(void) {
	if(this->is_ok()) {
		throw;
	}
	return std::move(this->value.err_value);
}

template<typename T, typename E>
inline T&& Result<T, E>::unwrap_or(T or_value) {
	return std::move(
		this->is_ok() ? this->value.ok_value : or_value
	);
}

template<typename T, typename E>
inline std::optional<T> Result<T, E>::ok(void) noexcept {
	if(this->is_err()) {
		return std::optional<T>();
	}
	return std::move(this->value.ok_value);
}

template<typename T, typename E>
inline std::optional<E> Result<T, E>::err(void) noexcept {
	if(this->is_ok()) {
		return std::optional<T>();
	}
	return std::move(this->value.err_value);
}

template<typename T, typename E>
inline bool Result<T, E>::is_ok(void) noexcept {
	return this->result_type == inner::ResultType::Ok;
}

template<typename T, typename E>
inline bool Result<T, E>::is_err(void) noexcept {
	return this->result_type == inner::ResultType::Err;
}
