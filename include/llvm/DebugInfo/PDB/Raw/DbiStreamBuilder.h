//===- DbiStreamBuilder.h - PDB Dbi Stream Creation -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_DEBUGINFO_PDB_RAW_PDBDBISTREAMBUILDER_H
#define LLVM_DEBUGINFO_PDB_RAW_PDBDBISTREAMBUILDER_H

#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Error.h"

#include "llvm/DebugInfo/Msf/ByteStream.h"
#include "llvm/DebugInfo/Msf/StreamReader.h"
#include "llvm/DebugInfo/PDB/PDBTypes.h"
#include "llvm/DebugInfo/PDB/Raw/PDBFile.h"
#include "llvm/DebugInfo/PDB/Raw/RawConstants.h"

namespace llvm {
namespace pdb {
class DbiStream;
class PDBFile;

class DbiStreamBuilder {
public:
  DbiStreamBuilder(BumpPtrAllocator &Allocator);

  DbiStreamBuilder(const DbiStreamBuilder &) = delete;
  DbiStreamBuilder &operator=(const DbiStreamBuilder &) = delete;

  void setVersionHeader(PdbRaw_DbiVer V);
  void setAge(uint32_t A);
  void setBuildNumber(uint16_t B);
  void setPdbDllVersion(uint16_t V);
  void setPdbDllRbld(uint16_t R);
  void setFlags(uint16_t F);
  void setMachineType(PDB_Machine M);

  uint32_t calculateSerializedLength() const;

  Error addModuleInfo(StringRef ObjFile, StringRef Module);
  Error addModuleSourceFile(StringRef Module, StringRef File);

  Expected<std::unique_ptr<DbiStream>> build(PDBFile &File);

private:
  uint32_t calculateModiSubstreamSize() const;
  uint32_t calculateFileInfoSubstreamSize() const;
  uint32_t calculateNamesBufferSize() const;

  Error generateModiSubstream();
  Error generateFileInfoSubstream();

  struct ModuleInfo {
    std::vector<StringRef> SourceFiles;
    StringRef Obj;
    StringRef Mod;
  };

  BumpPtrAllocator &Allocator;

  Optional<PdbRaw_DbiVer> VerHeader;
  uint32_t Age;
  uint16_t BuildNumber;
  uint16_t PdbDllVersion;
  uint16_t PdbDllRbld;
  uint16_t Flags;
  PDB_Machine MachineType;

  StringMap<std::unique_ptr<ModuleInfo>> ModuleInfos;
  std::vector<ModuleInfo *> ModuleInfoList;

  StringMap<uint32_t> SourceFileNames;

  msf::StreamRef NamesBuffer;
  msf::ByteStream<true> ModInfoBuffer;
  msf::ByteStream<true> FileInfoBuffer;
};
}
}

#endif
