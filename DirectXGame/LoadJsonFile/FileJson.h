// FileJson.h
#pragma once                 // 多重インクルードを防ぐ
#include "nlohmann/json.hpp" // JSONライブラリ
#include <KamataEngine.h>    // KamataEngineのヘッダーファイル (Vector3 など)
#include <fstream>           // ファイルストリーム
#include <iostream>          // 標準入出力
#include <sstream>           // 文字列ストリーム
#include <stdexcept>         // 例外処理
#include <string>            // 文字列
#include <vector>            // std::vector を使用するため

using json = nlohmann::json;  // nlohmann::json を json という名前で使用する
using namespace KamataEngine; // KamataEngineの名前空間を使用

namespace FileJson { // FileJsonという名前空間を定義

class FileAccessor {
public:
	/// <summary>
	/// コンストラクタ：JSONファイルを読み込む
	/// </summary>
	/// <param name="filename">ファイル名</param>
	FileAccessor(const std::string& filename);
	// デストラクタ
	~FileAccessor();

	/// <summary>
	/// JSONから値を読み込む関数
	/// </summary>
	/// <typeparam name="T">読み込むデータの型</typeparam>
	/// <param name="desiredClass">JSONファイルのクラス名</param>
	/// <param name="variableName">JSONファイルの変数名</param>
	/// <param name="defaultValue">値が存在しない場合のデフォルト値</param>
	/// <returns>読み込んだ値</returns>
	template<typename T> T Read(const std::string& desiredClass, const std::string& variableName, const T& defaultValue) const;

	/// <summary>
	/// JSONに値を書き込む関数
	/// </summary>
	/// <typeparam name="T">書き込むデータの型</typeparam>
	/// <param name="desiredClass">JSONファイルのクラス名</param>
	/// <param name="variableName">JSONファイルの変数名</param>
	/// <param name="value">書き込む値</param>
	template<typename T> void Write(const std::string& desiredClass, const std::string& variableName, const T& value);

	/// <summary>
	/// Vector3型データを読み込むための特殊化関数
	/// </summary>
	/// <param name="desiredClass">JSONファイルのクラス名</param>
	/// <param name="variableName">JSONファイルの変数名</param>
	/// <param name="defaultValue">値が存在しない場合のデフォルト値</param>
	/// <returns>読み込んだVector3型の値</returns>
	Vector3 ReadVector3(const std::string& desiredClass, const std::string& variableName, const Vector3& defaultValue) const;

	/// <summary>
	/// Vector3型データを書き込むための特殊化関数
	/// </summary>
	/// <param name="desiredClass">JSONファイルのクラス名</param>
	/// <param name="variableName">JSONファイルの変数名</param>
	/// <param name="value">書き込むVector3型の値</param>
	void WriteVector3(const std::string& desiredClass, const std::string& variableName, const Vector3& value);

	/// <summary>
	/// CSV形式のデータをJSONから読み込む関数
	/// </summary>
	/// <param name="desiredClass">JSONファイルのクラス名</param>
	/// <param name="variableName">JSONファイルの変数名</param>
	/// <returns>読み込んだCSVデータ</returns>
	std::vector<std::vector<int>> ReadCsvData(const std::string& desiredClass, const std::string& variableName) const;

	/// <summary>
	/// CSVデータをJSONに書き込む関数
	/// </summary>
	/// <param name="desiredClass">SONファイルのクラス名</param>
	/// <param name="variableName">JSONファイルの変数名</param>
	/// <param name="csvData">CSVデータ</param>
	/// <returns>書き込むCSVデータ</returns>
	void WriteCsvData(const std::string& desiredClass, const std::string& variableName, const std::vector<std::vector<int>>& csvData);

	/// <summary>
	/// JSONデータをファイルに保存する
	/// </summary>
	void Save();

private:
	std::string filename_; // JSONファイルの名前
	json jsonData_;        // JSONデータ

	/// <summary>
	/// ファイルからJSONデータをロードする
	/// </summary>
	void LoadJsonFromFile();
};

/// <summary>
/// 読み込み関数
/// </summary>
/// <typeparam name="T">読み込むデータの型</typeparam>
/// <param name="desiredClass">JSONファイルのクラス名</param>
/// <param name="variableName">JSONファイルの変数名</param>
/// <param name="defaultValue">値が存在しない場合のデフォルト値</param>
/// <returns>読み込んだ値</returns>
template<typename T> T FileAccessor::Read(const std::string& desiredClass, const std::string& variableName, const T& /*defaultValue*/) const {
	try {
		// 指定されたクラスと変数から値を読み込む
		if (jsonData_.contains(desiredClass) && jsonData_[desiredClass].contains(variableName)) {
			// 値を返す
			return jsonData_[desiredClass][variableName].get<T>();
			// キーが見つからない場合
		} else {
			// エラーメッセージを出力
			std::stringstream ss;
			ss << "Error: Key not found in JSON - Class: " << desiredClass << ", Variable: " << variableName;
			// 例外をスローする
			throw std::runtime_error(ss.str());
		}
		// 例外が発生した場合
	} catch (const nlohmann::json::type_error& e) {
		// エラーメッセージを出力
		std::stringstream ss;
		ss << "Error: JSON type error - " << e.what() << " Class: " << desiredClass << ", Variable: " << variableName;
		// 例外をスローする
		throw std::runtime_error(ss.str());
		// 例外が発生した場合
	} catch (const std::exception& e) {
		// エラーメッセージを出力
		std::stringstream ss;
		ss << "Error: Exception during JSON read - " << e.what() << " Class: " << desiredClass << ", Variable: " << variableName;
		// 例外をスローする
		throw std::runtime_error(ss.str());
	}
}

/// <summary>
/// 書き込み関数
/// </summary>
/// <typeparam name="T">書き込むデータの型</typeparam>
/// <param name="desiredClass">JSONファイルのクラス名</param>
/// <param name="variableName">JSONファイルの変数名</param>
/// <param name="value">書き込む値</param>
template<typename T> void FileAccessor::Write(const std::string& desiredClass, const std::string& variableName, const T& value) {
	try {
		// 指定されたクラスと変数に値を書き込む
		jsonData_[desiredClass][variableName] = value;
	} catch (const std::exception& e) {
		// 例外が発生した場合はエラーメッセージを出力
		std::cerr << "Error: Exception during JSON write - " << e.what() << std::endl;
	}
}

} // namespace FileJson