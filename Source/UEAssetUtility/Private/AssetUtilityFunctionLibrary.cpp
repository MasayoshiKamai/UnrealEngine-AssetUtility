// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetUtilityFunctionLibrary.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "HAL/IConsoleManager.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"

#include "EditorAssetLibrary.h"
#include "FileHelpers.h" 



// パッケージ名を指定してアセットを取得する
bool UAssetUtilityFunctionLibrary::GetAssetsByPackageName(const FName &PackageName, TArray<FAssetData> &OutAssetData, bool bIncludeOnlyOnDiskAssets)
{
    if (PackageName.IsNone())
    {
        return false;
    }

    FARFilter Filter;
    Filter.PackageNames.Add(PackageName);
    Filter.bIncludeOnlyOnDiskAssets = bIncludeOnlyOnDiskAssets;

    return IAssetRegistry::Get()->GetAssets(Filter, OutAssetData);
}

// 指定したパス以下のパッケージを取得する
bool UAssetUtilityFunctionLibrary::GetAssetsByPaths(const TArray<FName>& PackagePaths, TArray<FAssetData>& OutAssetData, bool bRecursivePaths, bool bIncludeOnlyOnDiskAssets)
{
    FARFilter Filter;
    Filter.bRecursivePaths = bRecursivePaths;
    Filter.bIncludeOnlyOnDiskAssets = bIncludeOnlyOnDiskAssets;

    if (!PackagePaths.IsEmpty())
    {
        Filter.PackagePaths.Append(PackagePaths);
    }

    return IAssetRegistry::Get()->GetAssets(Filter, OutAssetData);
}

// 指定したクラスを持つ全てのアセットを取得する
bool UAssetUtilityFunctionLibrary::GetAssetsByClass(FTopLevelAssetPath ClassPathName, TArray<FAssetData> &OutAssetData, bool bSearchSubClasses)
{
    if (!ClassPathName.IsValid())
    {
        return false;
    }

    FARFilter Filter;
    Filter.ClassPaths.Add(ClassPathName);
    Filter.bRecursiveClasses = bSearchSubClasses;

    return IAssetRegistry::Get()->GetAssets(Filter, OutAssetData);
}

// 指定したパッケージが依存するアセットを取得
bool UAssetUtilityFunctionLibrary::GetDependencies(const FName& PackageName, TArray<FAssetData>& OutDependencyAssetData, bool bIncludeOnlyOnDiskAssets)
{
    if (PackageName.IsNone())
    {
        return false;
    }

    TArray<FAssetIdentifier> TempAssetIdentifier;
    bool bResult = IAssetRegistry::Get()->GetDependencies(FAssetIdentifier(PackageName), TempAssetIdentifier);

    if (bResult)
    {
        for (const auto& AssetId : TempAssetIdentifier)
        {
            if (AssetId.PackageName != NAME_None)
            {
                TArray<FAssetData> TempAssetData;
                GetAssetsByPackageName(AssetId.PackageName, TempAssetData, bIncludeOnlyOnDiskAssets);

                if (!TempAssetData.IsEmpty())
                {
                    OutDependencyAssetData.Append(TempAssetData);
                }
            }
        }
    }

    return bResult;
}

// 指定したパッケージが参照するアセットを取得
bool UAssetUtilityFunctionLibrary::GetReferencers(const FName& PackageName, TArray<FAssetData>& OutDependencyAssetData, bool bIncludeOnlyOnDiskAssets)
{
    if (PackageName.IsNone())
    {
        return false;
    }

    TArray<FAssetIdentifier> TempAssetIdentifier;
    bool bResult = IAssetRegistry::Get()->GetReferencers(FAssetIdentifier(PackageName), TempAssetIdentifier);

    if (bResult) {
        for (const auto &AssetId : TempAssetIdentifier)
        {
            if (AssetId.PackageName != NAME_None) {
                TArray<FAssetData> TempAssetData;
                GetAssetsByPackageName(AssetId.PackageName, TempAssetData, bIncludeOnlyOnDiskAssets);

                if (!TempAssetData.IsEmpty()) {
                    OutDependencyAssetData.Append(TempAssetData);
                }
            }
        }
    }

    return bResult;
}

// ロード済みアセットを複製する
UObject*UAssetUtilityFunctionLibrary::DuplicateLoadedAsset(UObject* SrcAsset, const FName& DstPackageName)
{
    if (!IsValid(SrcAsset) || DstPackageName.IsNone()) {
        return nullptr;
    }

    return UEditorAssetLibrary::DuplicateLoadedAsset(SrcAsset, DstPackageName.ToString());
}

// アセットを複製する
UObject* UAssetUtilityFunctionLibrary::DuplicateAsset(const FName& SrcPackageName, const FName& DstPackageName)
{
    if (SrcPackageName.IsNone() || DstPackageName.IsNone())
    {
        return nullptr;
    }

    const FString SrcPackageNameStr = SrcPackageName.ToString();
    const FString DstPackageNameStr = DstPackageName.ToString();

    return UEditorAssetLibrary::DuplicateAsset(SrcPackageNameStr, DstPackageNameStr);
}

#if WITH_EDITOR

// 指定パス以下のアセットを取得
static FAutoConsoleCommand TestGetAssetByPaths = FAutoConsoleCommand(
    TEXT("Test.AssetUtility.GetAssetsByPaths"),
    TEXT("Test.AssetUtility.GetAssetsByPaths [PackagePath...]"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        TArray<FName> PackagePaths;

        if (0 < Args.Num())
        {
            for (auto &Arg : Args)
            {
                PackagePaths.Add(FName(Arg));
            }
        }
        
        const bool bRecursivePaths = true;
        const bool bIncludeOnlyOnDiskAssets = false;

        TArray<FAssetData> OutAssetData;
        if (UAssetUtilityFunctionLibrary::GetAssetsByPaths(PackagePaths, OutAssetData, bRecursivePaths, bIncludeOnlyOnDiskAssets))
        {
            int32 Index = 0;
            for (auto& AssetData : OutAssetData)
            {
                UE_LOG(LogTemp, Log, TEXT("Found asset[%d]: '%s' %s"), Index++, *(AssetData.AssetClassPath.GetAssetName().ToString()), *(AssetData.PackageName.ToString()));
            }
        }
    }));

// 全てのマテリアルアセットを取得
static FAutoConsoleCommand TestGetAllMaterialAssets = FAutoConsoleCommand(
    TEXT("Test.AssetUtility.GetAllMaterialAssets"),
    TEXT("Get all material assets"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        const bool bSearchSubClasses = true;

        TArray<FAssetData> OutAssetData;
        if (UAssetUtilityFunctionLibrary::GetAssetsByClass(UMaterial::StaticClass()->GetClassPathName(), OutAssetData, bSearchSubClasses))
        {
            int32 Index = 0;
            for (auto &AssetData : OutAssetData) {
                UE_LOG(LogTemp, Log, TEXT("Found asset[%d]: '%s' %s"), Index++, *(AssetData.AssetClassPath.GetAssetName().ToString()), *(AssetData.PackageName.ToString()));
            }
        }
    }));

// 全ての静的メッシュアセットを取得
static FAutoConsoleCommand TestGetAllStaticMeshAssets = FAutoConsoleCommand(
    TEXT("Test.AssetUtility.GetAllStaticMeshAssets"),
    TEXT("Get all static mesh assets"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        const bool bSearchSubClasses = true;

        TArray<FAssetData> OutAssetData;
        if (UAssetUtilityFunctionLibrary::GetAssetsByClass(UStaticMesh::StaticClass()->GetClassPathName(), OutAssetData, bSearchSubClasses))
        {
            int32 Index = 0;
            for (auto &AssetData : OutAssetData) {
                UE_LOG(LogTemp, Log, TEXT("Found asset[%d]: '%s' %s"), Index++, *(AssetData.AssetClassPath.GetAssetName().ToString()), *(AssetData.PackageName.ToString()));
            }
        }
    }));

// 依存するアセットを全て取得
static FAutoConsoleCommand TestGetDependencies = FAutoConsoleCommand(
    TEXT("Test.AssetUtility.GetDependencies"),
    TEXT("Test.AssetUtility.GetDependencies PackageName"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (0 == Args.Num()) {
            return;
        }

        const FName PackageName(Args[0]);
        const bool bIncludeOnlyOnDiskAssets = false;

        TArray<FAssetData> OutAssetData;
        if (UAssetUtilityFunctionLibrary::GetDependencies(PackageName, OutAssetData, bIncludeOnlyOnDiskAssets)) {
            int32 Index = 0;
            for (auto &AssetData : OutAssetData) {
                UE_LOG(LogTemp, Log, TEXT("Found dependency asset[%d]: '%s' %s"), Index++, *(AssetData.AssetClassPath.GetAssetName().ToString()), *(AssetData.PackageName.ToString()));
            }
        }
    }));

// 参照しているアセットを全て取得
static FAutoConsoleCommand TestGetReferencers = FAutoConsoleCommand(
    TEXT("Test.AssetUtility.GetReferencers"),
    TEXT("Test.AssetUtility.GetReferencers PackageName"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) {
    if (0 == Args.Num()) {
        return;
    }

    const FName PackageName(Args[0]);
    const bool bIncludeOnlyOnDiskAssets = false;

    TArray<FAssetData> OutAssetData;
    if (UAssetUtilityFunctionLibrary::GetReferencers(PackageName, OutAssetData, bIncludeOnlyOnDiskAssets)) {
        int32 Index = 0;
        for (auto &AssetData : OutAssetData) {
            UE_LOG(LogTemp, Log, TEXT("Found reference asset[%d]: '%s' %s"), Index++, *(AssetData.AssetClassPath.GetAssetName().ToString()), *(AssetData.PackageName.ToString()));
        }
    }
}));

#endif // WITH_EDITOR
