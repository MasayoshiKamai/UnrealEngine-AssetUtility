// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetUtilityFunctionLibrary.generated.h"


UCLASS()
class UEASSETUTILITY_API UAssetUtilityFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// パッケージ名を指定してアセットを取得する
	// @param[in]  PackageName              取得するアセットのパッケージ名
	// @param[out] OutAssetData             見つかったアセットの各種情報
	// @param[in]  bIncludeOnlyOnDiskAssets Trueの場合ディスク上に格納されているデータのみを収集する
	// @return 取得に成功すればTrue、そうでなければFalseを返す
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting|Asset")
	static bool GetAssetsByPackageName(const FName& PackageName, TArray<FAssetData> &OutAssetData, bool bIncludeOnlyOnDiskAssets = false);

	// 指定したパス以下のパッケージを取得する
	// @param[in]  PackagePaths             パッケージを検索する起点パス
	// @param[out] OutAssetData             見つかったアセットの各種情報
	// @param[in]  bRecursivePaths          PackagePathsで指定したパス以下を再帰的に検索するか？
	// @param[in]  bIncludeOnlyOnDiskAssets Trueの場合ディスク上に格納されているデータのみを収集する
	// @return 取得に成功すればTrue、そうでなければFalseを返す
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting|Asset")
	static bool GetAssetsByPaths(const TArray<FName> &PackagePaths, TArray<FAssetData> &OutAssetData, bool bRecursivePaths = false, bool bIncludeOnlyOnDiskAssets = false);
	
	// 指定したクラスを持つ全てのアセットを取得する
	// @param[in]  ClassPathName     指定するクラスパス名
	// @param[out] OutAssetData      見つかったアセットの各種情報
	// @param[in]  bSearchSubClasses ClassPathNameで指定したクラスの派生クラスも検索するか？
	// @return 取得に成功すればTrue、そうでなければFalseを返す
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting|Asset")
	static bool GetAssetsByClass(FTopLevelAssetPath ClassPathName, TArray<FAssetData> &OutAssetData, bool bSearchSubClasses = false);

	// 指定したパッケージが依存するアセットを取得
	// @param[in]  PackageName              依存元アセットのパッケージ名
	// @param[out] OutAssetData             見つかった依存先アセットの各種情報
	// @param[in]  bIncludeOnlyOnDiskAssets Trueの場合ディスク上に格納されているデータのみを収集する
	// @return 取得に成功すればTrue、そうでなければFalseを返す
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting|Asset")
	static bool GetDependencies(const FName &PackageName, TArray<FAssetData>& OutDependencyAssetData, bool bIncludeOnlyOnDiskAssets = false);

	// 指定したパッケージが参照するアセットを取得
	// @param[in]  PackageName              参照元アセットのパッケージ名
	// @param[out] OutAssetData             見つかった参照先アセットの各種情報
	// @param[in]  bIncludeOnlyOnDiskAssets Trueの場合ディスク上に格納されているデータのみを収集する
	// @return 取得に成功すればTrue、そうでなければFalseを返す
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting|Asset")
	static bool GetReferencers(const FName &PackageName, TArray<FAssetData> &OutDependencyAssetData, bool bIncludeOnlyOnDiskAssets = false);

	// ロード済みアセットを複製する
	// @param[in] SrcAsset       複製元アセットのポインタ
	// @param[in] DstPackageName 複製先アセットのパッケージ名
	// @return 複製したアセットのインスタンスを返す
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting|Asset")
	static UObject* DuplicateLoadedAsset(UObject* SrcAsset, const FName& DstPackageName);

	// アセットを複製する
	// @param[in] SrcPackageName 複製元アセットのパッケージ名
	// @param[in] DstPackageName 複製先アセットのパッケージ名
	// @return 複製したアセットのインスタンスを返す
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting|Asset")
	static UObject* DuplicateAsset(const FName& SrcPackageName, const FName& DstPackageName);
};
