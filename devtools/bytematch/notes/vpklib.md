# vpklib: per-function status vs retail depot 223

Generated from devtools/bytematch on the --retail-osx32 build (linked dylib vs retail dylib, scope = retail debug-map objects).
"with needed" is the per-object status once the shared header changes in lane-mod-filesystem.md land (lane-mod-filesystem-scratch/needed.patch).

## crypto (172/176)

| function | status | with needed | note |
|---|---|---|---|
| `CCrypto::RSAVerifySignatureSHA256(unsigned char const*, unsigned int, unsigned char const*, unsigned int, unsigned char const*, unsigned int)` | masked | masked |  |
| `CGlobalInitConstructor::CGlobalInitConstructor()` | masked | masked |  |
| `CTSList<CryptoPP::AutoSeededX917RNG<CryptoPP::Rijndael>>::~CTSList()` | exact | masked |  |
| `CTSList<CryptoPP::AutoSeededX917RNG<CryptoPP::Rijndael>>::~CTSList()` | masked | masked |  |
| `CryptoPP::ASN1Object::BEREncode(CryptoPP::BufferedTransformation&) const` | exact | exact |  |
| `CryptoPP::ASN1Object::~ASN1Object()` | exact | exact |  |
| `CryptoPP::ASN1Object::~ASN1Object()` | masked | masked |  |
| `CryptoPP::Algorithm::AlgorithmName() const` | masked | masked |  |
| `CryptoPP::AlgorithmImpl<CryptoPP::IteratedHash<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, CryptoPP::HashTransformation>, CryptoP` | masked | masked |  |
| `CryptoPP::AlgorithmImpl<CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, CryptoPP::TwoBases<Cry` | masked | masked |  |
| `CryptoPP::AlgorithmImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoPP::RSA, int>>::AlgorithmName() const` | exact | masked |  |
| `CryptoPP::AlgorithmParameters& CryptoPP::AlgorithmParameters::operator()<CryptoPP::ConstByteArrayParameter>(char const*, CryptoPP::ConstByteArrayParam` | masked | masked |  |
| `CryptoPP::AlgorithmParametersBase::ParameterNotUsed::ParameterNotUsed(char const*)` | masked | masked |  |
| `CryptoPP::AlgorithmParametersBase::ParameterNotUsed::~ParameterNotUsed()` | masked | masked |  |
| `CryptoPP::AlgorithmParametersBase::ParameterNotUsed::~ParameterNotUsed()` | masked | masked |  |
| `CryptoPP::AlgorithmParametersBase::~AlgorithmParametersBase()` | masked | masked |  |
| `CryptoPP::AlgorithmParametersBase::~AlgorithmParametersBase()` | masked | masked |  |
| `CryptoPP::AlgorithmParametersBase::~AlgorithmParametersBase()` | masked | masked |  |
| `CryptoPP::BlockCipher::GetAlgorithm() const` | exact | exact |  |
| `CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>::IsForwardTransformation() const` | exact | exact |  |
| `CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>::~BlockCipherFinal()` | masked | masked |  |
| `CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>::~BlockCipherFinal()` | masked | masked |  |
| `CryptoPP::BlockCipherImpl<CryptoPP::Rijndael_Info, CryptoPP::BlockCipher>::BlockSize() const` | exact | exact |  |
| `CryptoPP::BlockTransformation::IsPermutation() const` | exact | exact |  |
| `CryptoPP::BlockTransformation::OptimalNumberOfParallelBlocks() const` | exact | exact |  |
| `CryptoPP::BufferedTransformation::CanModifyInput() const` | exact | exact |  |
| `CryptoPP::BufferedTransformation::CreatePutSpace(unsigned long&)` | exact | exact |  |
| `CryptoPP::BufferedTransformation::GetAutoSignalPropagation() const` | exact | exact |  |
| `CryptoPP::BufferedTransformation::GetNextMessageSeries()` | exact | exact |  |
| `CryptoPP::BufferedTransformation::IsolatedInitialize(CryptoPP::NameValuePairs const&)` | masked | masked |  |
| `CryptoPP::BufferedTransformation::IsolatedMessageSeriesEnd(bool)` | exact | exact |  |
| `CryptoPP::BufferedTransformation::NumberOfMessageSeries() const` | exact | exact |  |
| `CryptoPP::BufferedTransformation::NumberOfMessagesInThisSeries() const` | exact | exact |  |
| `CryptoPP::BufferedTransformation::PutModifiable2(unsigned char*, unsigned long, int, bool)` | exact | exact |  |
| `CryptoPP::BufferedTransformation::SetAutoSignalPropagation(int)` | exact | exact |  |
| `CryptoPP::CannotFlush::~CannotFlush()` | missing | masked | Crypto++ template instance |
| `CryptoPP::Clonable::Clone() const` | masked | masked |  |
| `CryptoPP::ClonableImpl<CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>, CryptoPP::Rijndael::Enc>::Clone() const` | masked | masked |  |
| `CryptoPP::ClonableImpl<CryptoPP::SHA256, CryptoPP::AlgorithmImpl<CryptoPP::IteratedHash<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64` | masked | masked |  |
| `CryptoPP::Exception::what() const` | exact | exact |  |
| `CryptoPP::Exception::~Exception()` | masked | masked |  |
| `CryptoPP::Exception::~Exception()` | masked | masked |  |
| `CryptoPP::Filter::Attachable()` | exact | exact |  |
| `CryptoPP::Filter::ShouldPropagateMessageEnd() const` | exact | exact |  |
| `CryptoPP::Filter::ShouldPropagateMessageSeriesEnd() const` | exact | exact |  |
| `CryptoPP::HashTransformation::BlockSize() const` | exact | exact |  |
| `CryptoPP::HashTransformation::CalculateDigest(unsigned char*, unsigned char const*, unsigned long)` | exact | exact |  |
| `CryptoPP::HashTransformation::CalculateTruncatedDigest(unsigned char*, unsigned long, unsigned char const*, unsigned long)` | exact | exact |  |
| `CryptoPP::HashTransformation::CreateUpdateSpace(unsigned long&)` | exact | exact |  |
| `CryptoPP::HashTransformation::Final(unsigned char*)` | exact | exact |  |
| `CryptoPP::HashTransformation::OptimalBlockSize() const` | exact | exact |  |
| `CryptoPP::HashTransformation::Restart()` | exact | exact |  |
| `CryptoPP::HashTransformation::Verify(unsigned char const*)` | exact | exact |  |
| `CryptoPP::HashTransformation::VerifyDigest(unsigned char const*, unsigned char const*, unsigned long)` | exact | exact |  |
| `CryptoPP::HashTransformation::VerifyTruncatedDigest(unsigned char const*, unsigned long, unsigned char const*, unsigned long)` | exact | exact |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::ChannelMessageSeriesEnd(std::string const&, int, bool)` | masked | masked |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::ChannelPut2(std::string const&, unsigned char const*, unsigned long, int, bool)` | masked | masked |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::InputRejected::InputRejected()` | masked | masked |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::InputRejected::~InputRejected()` | masked | masked |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::InputRejected::~InputRejected()` | masked | masked |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::IsolatedFlush(bool, bool)` | exact | exact |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::IsolatedMessageSeriesEnd(bool)` | masked | masked |  |
| `CryptoPP::InputRejecting<CryptoPP::Filter>::Put2(unsigned char const*, unsigned long, int, bool)` | masked | masked |  |
| `CryptoPP::InvalidArgument::~InvalidArgument()` | masked | masked |  |
| `CryptoPP::InvalidArgument::~InvalidArgument()` | masked | masked |  |
| `CryptoPP::IteratedHash<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, CryptoPP::HashTransformation>::BlockSize() const` | exact | exact |  |
| `CryptoPP::IteratedHash<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, CryptoPP::HashTransformation>::DataBuf()` | exact | exact |  |
| `CryptoPP::IteratedHash<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, CryptoPP::HashTransformation>::GetByteOrder() const` | exact | exact |  |
| `CryptoPP::IteratedHash<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, CryptoPP::HashTransformation>::~IteratedHash()` | masked | masked |  |
| `CryptoPP::IteratedHash<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, CryptoPP::HashTransformation>::~IteratedHash()` | masked | masked |  |
| `CryptoPP::IteratedHashWithStaticTransform<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, 32u, CryptoPP::SHA256, 32u, true>::DigestSi` | exact | exact |  |
| `CryptoPP::IteratedHashWithStaticTransform<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, 32u, CryptoPP::SHA256, 32u, true>::HashEndi` | masked | masked |  |
| `CryptoPP::IteratedHashWithStaticTransform<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, 32u, CryptoPP::SHA256, 32u, true>::Init()` | masked | masked |  |
| `CryptoPP::IteratedHashWithStaticTransform<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, 32u, CryptoPP::SHA256, 32u, true>::Iterated` | masked | masked |  |
| `CryptoPP::IteratedHashWithStaticTransform<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, 32u, CryptoPP::SHA256, 32u, true>::StateBuf` | exact | exact |  |
| `CryptoPP::IteratedHashWithStaticTransform<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, 32u, CryptoPP::SHA256, 32u, true>::~Iterate` | masked | masked |  |
| `CryptoPP::IteratedHashWithStaticTransform<unsigned int, CryptoPP::EnumToType<CryptoPP::ByteOrder, 1>, 64u, 32u, CryptoPP::SHA256, 32u, true>::~Iterate` | masked | masked |  |
| `CryptoPP::NameValuePairs::ThrowIfTypeMismatch(char const*, std::type_info const&, std::type_info const&)` | masked | masked |  |
| `CryptoPP::NameValuePairs::ValueTypeMismatch::ValueTypeMismatch(std::string const&, std::type_info const&, std::type_info const&)` | masked | masked |  |
| `CryptoPP::NameValuePairs::ValueTypeMismatch::~ValueTypeMismatch()` | masked | masked |  |
| `CryptoPP::NameValuePairs::ValueTypeMismatch::~ValueTypeMismatch()` | masked | masked |  |
| `CryptoPP::NotImplemented::~NotImplemented()` | masked | masked |  |
| `CryptoPP::NotImplemented::~NotImplemented()` | masked | masked |  |
| `CryptoPP::PK_FinalTemplate<CryptoPP::TF_VerifierImpl<CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoP` | masked | masked |  |
| `CryptoPP::PK_FinalTemplate<CryptoPP::TF_VerifierImpl<CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoP` | masked | masked |  |
| `CryptoPP::PK_MessageAccumulator::DigestSize() const` | masked | masked |  |
| `CryptoPP::PK_MessageAccumulator::TruncatedFinal(unsigned char*, unsigned long)` | masked | masked |  |
| `CryptoPP::PK_MessageAccumulatorBase::PK_MessageAccumulatorBase()` | masked | masked |  |
| `CryptoPP::PK_MessageAccumulatorBase::Update(unsigned char const*, unsigned long)` | exact | exact |  |
| `CryptoPP::PK_MessageAccumulatorBase::~PK_MessageAccumulatorBase()` | masked | masked |  |
| `CryptoPP::PK_MessageAccumulatorBase::~PK_MessageAccumulatorBase()` | exact | masked |  |
| `CryptoPP::PK_MessageAccumulatorBase::~PK_MessageAccumulatorBase()` | masked | masked |  |
| `CryptoPP::PK_MessageAccumulatorImpl<CryptoPP::SHA256>::AccessHash()` | exact | exact |  |
| `CryptoPP::PK_MessageAccumulatorImpl<CryptoPP::SHA256>::~PK_MessageAccumulatorImpl()` | masked | masked |  |
| `CryptoPP::PK_MessageAccumulatorImpl<CryptoPP::SHA256>::~PK_MessageAccumulatorImpl()` | masked | masked |  |
| `CryptoPP::PK_SignatureMessageEncodingMethod::AllowNonrecoverablePart() const` | masked | masked |  |
| `CryptoPP::PK_SignatureScheme::MaxSignatureLength(unsigned long) const` | exact | exact |  |
| `CryptoPP::PK_SignatureScheme::SignatureUpfront() const` | exact | exact |  |
| `CryptoPP::PK_SignatureScheme::~PK_SignatureScheme()` | exact | exact |  |
| `CryptoPP::PK_SignatureScheme::~PK_SignatureScheme()` | masked | masked |  |
| `CryptoPP::PublicKeyAlgorithm::AccessMaterial()` | exact | exact |  |
| `CryptoPP::PublicKeyAlgorithm::GetMaterial() const` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterface::IsValidKeyLength(unsigned long) const` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterface::MaxIVLength() const` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterface::MinIVLength() const` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterface::Resynchronize(unsigned char const*, int)` | masked | masked |  |
| `CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, CryptoPP::TwoBases<CryptoPP::BlockCipher, Cryp` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, CryptoPP::TwoBases<CryptoPP::BlockCipher, Cryp` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, CryptoPP::TwoBases<CryptoPP::BlockCipher, Cryp` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, CryptoPP::TwoBases<CryptoPP::BlockCipher, Cryp` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, CryptoPP::TwoBases<CryptoPP::BlockCipher, Cryp` | exact | exact |  |
| `CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, CryptoPP::TwoBases<CryptoPP::BlockCipher, Cryp` | exact | exact |  |
| `CryptoPP::Singleton<CryptoPP::PKCS1v15_SignatureMessageEncodingMethod, CryptoPP::NewObject<CryptoPP::PKCS1v15_SignatureMessageEncodingMethod>, 0>::Ref` | masked | masked |  |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::GetAutoSignalPropagation() const` | exact | exact |  |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::IsolatedInitialize(CryptoPP::NameValuePairs const&)` | exact | exact |  |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::Pump2(unsigned long long&, bool)` | differs | differs | Crypto++ template instance |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::PumpAll2(bool)` | differs | differs | Crypto++ template instance |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::PumpMessages2(unsigned int&, bool)` | differs | differs | Crypto++ template instance |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::SetAutoSignalPropagation(int)` | exact | exact |  |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::SourceExhausted() const` | exact | exact |  |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::SourceTemplate(CryptoPP::BufferedTransformation*)` | masked | masked |  |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::~SourceTemplate()` | masked | masked |  |
| `CryptoPP::SourceTemplate<CryptoPP::StringStore>::~SourceTemplate()` | masked | masked |  |
| `CryptoPP::StringSource::StringSource(unsigned char const*, unsigned long, bool, CryptoPP::BufferedTransformation*)` | masked | masked |  |
| `CryptoPP::StringSource::~StringSource()` | masked | masked |  |
| `CryptoPP::StringSource::~StringSource()` | masked | masked |  |
| `CryptoPP::StringStore::StringStore(char const*)` | masked | masked |  |
| `CryptoPP::TF_ObjectImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoPP:` | exact | exact |  |
| `CryptoPP::TF_ObjectImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoPP:` | exact | exact |  |
| `CryptoPP::TF_ObjectImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoPP:` | masked | masked |  |
| `CryptoPP::TF_ObjectImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoPP:` | masked | masked |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | exact | exact |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | exact | exact |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | masked | masked |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | exact | masked |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | exact | exact |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | exact | exact |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | exact | exact |  |
| `CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, Crypt` | masked | masked |  |
| `CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoPP::RSA, int>::StaticAlgorithmName()` | masked | masked |  |
| `CryptoPP::TF_SignatureSchemeBase<CryptoPP::PK_Verifier, CryptoPP::TF_Base<CryptoPP::TrapdoorFunction, CryptoPP::PK_SignatureMessageEncodingMethod>>::A` | exact | masked |  |
| `CryptoPP::TF_SignatureSchemeBase<CryptoPP::PK_Verifier, CryptoPP::TF_Base<CryptoPP::TrapdoorFunction, CryptoPP::PK_SignatureMessageEncodingMethod>>::I` | exact | exact |  |
| `CryptoPP::TF_SignatureSchemeBase<CryptoPP::PK_Verifier, CryptoPP::TF_Base<CryptoPP::TrapdoorFunction, CryptoPP::PK_SignatureMessageEncodingMethod>>::M` | exact | masked |  |
| `CryptoPP::TF_SignatureSchemeBase<CryptoPP::PK_Verifier, CryptoPP::TF_Base<CryptoPP::TrapdoorFunction, CryptoPP::PK_SignatureMessageEncodingMethod>>::M` | exact | exact |  |
| `CryptoPP::TF_SignatureSchemeBase<CryptoPP::PK_Verifier, CryptoPP::TF_Base<CryptoPP::TrapdoorFunction, CryptoPP::PK_SignatureMessageEncodingMethod>>::M` | masked | masked |  |
| `CryptoPP::TF_SignatureSchemeBase<CryptoPP::PK_Verifier, CryptoPP::TF_Base<CryptoPP::TrapdoorFunction, CryptoPP::PK_SignatureMessageEncodingMethod>>::R` | exact | exact |  |
| `CryptoPP::TF_SignatureSchemeBase<CryptoPP::PK_Verifier, CryptoPP::TF_Base<CryptoPP::TrapdoorFunction, CryptoPP::PK_SignatureMessageEncodingMethod>>::S` | masked | masked |  |
| `CryptoPP::TrapdoorFunction::ApplyRandomizedFunction(CryptoPP::RandomNumberGenerator&, CryptoPP::Integer const&) const` | exact | exact |  |
| `CryptoPP::TrapdoorFunction::IsRandomized() const` | exact | exact |  |
| `CryptoPP::TrapdoorFunction::~TrapdoorFunction()` | exact | exact |  |
| `CryptoPP::TrapdoorFunction::~TrapdoorFunction()` | masked | masked |  |
| `CryptoPP::TrapdoorFunctionBounds::MaxImage() const` | masked | masked |  |
| `CryptoPP::TrapdoorFunctionBounds::MaxPreimage() const` | masked | masked |  |
| `CryptoPP::simple_ptr<CryptoPP::PKCS1v15_SignatureMessageEncodingMethod>::~simple_ptr()` | exact | exact |  |
| `__GLOBAL__I_a` | masked | masked |  |
| `non-virtual thunk to CryptoPP::AlgorithmImpl<CryptoPP::SimpleKeyingInterfaceImpl<CryptoPP::TwoBases<CryptoPP::BlockCipher, CryptoPP::Rijndael_Info>, C` | masked | masked |  |
| `non-virtual thunk to CryptoPP::AlgorithmImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SS<CryptoPP::PKCS1v15, CryptoPP::SHA256, CryptoPP::RSA, int>>::Al` | exact | masked |  |
| `non-virtual thunk to CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>::IsForwardTransformation() const` | exact | exact |  |
| `non-virtual thunk to CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>::~BlockCipherFinal()` | masked | masked |  |
| `non-virtual thunk to CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>::~BlockCipherFinal()` | masked | masked |  |
| `non-virtual thunk to CryptoPP::BlockCipherImpl<CryptoPP::Rijndael_Info, CryptoPP::BlockCipher>::BlockSize() const` | exact | exact |  |
| `non-virtual thunk to CryptoPP::ClonableImpl<CryptoPP::BlockCipherFinal<(CryptoPP::CipherDir)0, CryptoPP::Rijndael::Enc>, CryptoPP::Rijndael::Enc>::Clo` | masked | masked |  |
| `non-virtual thunk to CryptoPP::PK_FinalTemplate<CryptoPP::TF_VerifierImpl<CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Cryp` | masked | masked |  |
| `non-virtual thunk to CryptoPP::PK_FinalTemplate<CryptoPP::TF_VerifierImpl<CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Cryp` | masked | masked |  |
| `non-virtual thunk to CryptoPP::SourceTemplate<CryptoPP::StringStore>::~SourceTemplate()` | masked | masked |  |
| `non-virtual thunk to CryptoPP::SourceTemplate<CryptoPP::StringStore>::~SourceTemplate()` | masked | masked |  |
| `non-virtual thunk to CryptoPP::StringSource::~StringSource()` | masked | masked |  |
| `non-virtual thunk to CryptoPP::StringSource::~StringSource()` | masked | masked |  |
| `non-virtual thunk to CryptoPP::TF_ObjectImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Crypto` | masked | masked |  |
| `non-virtual thunk to CryptoPP::TF_ObjectImpl<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Crypto` | masked | masked |  |
| `non-virtual thunk to CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Cr` | exact | exact |  |
| `non-virtual thunk to CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Cr` | exact | masked |  |
| `non-virtual thunk to CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Cr` | exact | exact |  |
| `non-virtual thunk to CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Cr` | exact | exact |  |
| `non-virtual thunk to CryptoPP::TF_ObjectImplBase<CryptoPP::TF_VerifierBase, CryptoPP::TF_SignatureSchemeOptions<CryptoPP::TF_SS<CryptoPP::PKCS1v15, Cr` | exact | exact |  |
| `std::basic_string<char, std::char_traits<char>, std::allocator<char>> std::operator+<char, std::char_traits<char>, std::allocator<char>>(char const*, ` | masked | masked |  |

## packedstore (43/65)

| function | status | with needed | note |
|---|---|---|---|
| `CPackedStore::BTestDirectoryHash()` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::BTestMasterChunkHash()` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::BuildFindFirstCache()` | differs | masked | size retail 1152 vs ours 916 |
| `CPackedStore::BuildHashTables()` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::CPackedStore(char const*, char*, IBaseFileSystem*, bool)` | differs | masked | size retail 4918 vs ours 4982 |
| `CPackedStore::CheckSignature(int, void const*) const` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::FindFileEntry(char const*, char const*, char const*, unsigned char**, unsigned char**)` | masked | masked |  |
| `CPackedStore::FindFileHashFraction(int, int, ChunkHashFraction_t&)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::GetFileAndDirLists(CUtlStringList&, CUtlStringList&, bool)` | exact | exact |  |
| `CPackedStore::GetFileAndDirLists(char const*, CUtlStringList&, CUtlStringList&, bool)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::GetFileHandle(int)` | masked | masked |  |
| `CPackedStore::GetFileList(char const*, CUtlStringList&, bool, bool)` | masked | masked |  |
| `CPackedStore::GetHandleForHashingFiles()` | exact | exact |  |
| `CPackedStore::GetPackFileLoadErrorSummary(CUtlString&)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::GetPackFileName(CPackedStoreFileHandle&, char*, int) const` | masked | masked |  |
| `CPackedStore::HashAllChunkFiles()` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::HashChunkFile(int)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::HashEntirePackFile(CPackedStoreFileHandle&, long long&, int, int, FileHash_t&)` | masked | masked |  |
| `CPackedStore::Init()` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStore::IsEmpty() const` | exact | exact |  |
| `CPackedStore::OpenFile(char const*)` | masked | masked |  |
| `CPackedStore::ReadData(CPackedStoreFileHandle&, void*, int)` | masked | masked |  |
| `CPackedStore::~CPackedStore()` | differs | masked | size retail 3274 vs ours 3386 |
| `CPackedStoreReadCache::BCanSatisfyFromReadCacheInternal(unsigned char*, CPackedStoreFileHandle&, FileHandleTracker_t&, int, int, int&)` | differs | differs | size retail 1017 vs ours 804 |
| `CPackedStoreReadCache::CPackedStoreReadCache(IBaseFileSystem*)` | masked | masked |  |
| `CPackedStoreReadCache::CheckMd5Result(CachedVPKRead_t&)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CPackedStoreReadCache::FindBufferToUse()` | exact | exact |  |
| `CTSQueue<CachedVPKRead_t, false>::CTSQueue()` | masked | masked |  |
| `CTSQueue<CachedVPKRead_t, false>::Pop()` | masked | masked |  |
| `CTSQueue<CachedVPKRead_t, false>::Purge()` | masked | masked |  |
| `CTSQueue<CachedVPKRead_t, false>::~CTSQueue()` | masked | masked |  |
| `CUtlBlockMemory<unsigned char, int>::ChangeSize(int)` | differs | masked | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CUtlBlockMemory<unsigned char, int>::Init(int, int)` | differs | masked | size retail 203 vs ours 189 |
| `CUtlMemory<ChunkHashFraction_t, int>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsigned short>, unsigned short>::Grow(int)` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | exact | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CUtlMap<int, CUtlStringList*, unsigned short>::Node_t, unsigned short, CUtlMap<int, CUtlStringList*, unsigned short>::CKeyLess, CUtlMemory<` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlRBTree<CachedVPKRead_t, unsigned short, bool (*)(CachedVPKRead_t const&, CachedVPKRead_t const&), CUtlMemory<UtlRBTreeNode_t<CachedVPKRead_t, unsi` | masked | masked |  |
| `CUtlSortVector<ChunkHashFraction_t, ChunkHashFractionLess_t, CUtlVector<ChunkHashFraction_t, CUtlMemory<ChunkHashFraction_t, int>>>::Insert(ChunkHashF` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `CUtlStringList::SortFunc(char* const*, char* const*)` | missing | masked |  |
| `CUtlVector<unsigned char, CUtlBlockMemory<unsigned char, int>>::InsertBefore(int, unsigned char const&)` | differs | masked | size retail 207 vs ours 181 |
| `CachedVPKRead_t::Less(CachedVPKRead_t const&, CachedVPKRead_t const&)` | exact | exact |  |
| `SplitFileComponents(char const*, char*, char*, char*)` | masked | masked |  |
| `int CUtlSortVector<ChunkHashFraction_t, ChunkHashFractionLess_t, CUtlVector<ChunkHashFraction_t, CUtlMemory<ChunkHashFraction_t, int>>>::Find<ChunkHas` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |
| `int CUtlSortVector<ChunkHashFraction_t, ChunkHashFractionLess_t, CUtlVector<ChunkHashFraction_t, CUtlMemory<ChunkHashFraction_t, int>>>::FindLessOrEqu` | differs | differs | same size: member offsets (CBaseFileSystem/CQueuedLoader/CPackedStore layout) |


## leftovers lane

| function | status | note |
|---|---|---|
| `CPackedStoreReadCache::BCanSatisfyFromReadCacheInternal` | edited | retail `ReadCacheLine` submits the MD5 request without the `m_pFileTracker` null check; that version is cheap enough that clang inlines it here (retail has no out-of-line `ReadCacheLine`). 65/65 packedstore vs filesystem_stdio |
| `CryptoPP::SourceTemplate<StringStore>::Pump2/PumpMessages2/PumpAll2` | differs | Crypto++ header version (thirdparty) |
