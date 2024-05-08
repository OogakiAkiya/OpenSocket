#ifndef WRAPPEROPENSSL_UTILITY_H
#define WRAPPEROPENSSL_UTILITY_H

namespace WrapperOpenSSL {
/**
 *  @brief MD5のハッシュ値を取得する
 *
 *  @param _data	ハッシュ化させるデータ
 **/
std::string createMD5Hash(const std::string _data);

}  // namespace WrapperOpenSSL

#endif