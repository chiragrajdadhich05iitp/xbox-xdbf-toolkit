# Xbox 360 XDBF Metadata Engine & Binary Serialization Pipeline

<div align="center">

![C++ Standard](https://img.shields.io/badge/ISO_C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![UI Framework](https://img.shields.io/badge/GUI-Qt_5.15%2B_QtWidgets-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![Toolchain](https://img.shields.io/badge/Toolchain-MSYS2_UCRT64-2F3238?style=for-the-badge)
![License](https://img.shields.io/badge/License-GPL_v3.0-blue?style=for-the-badge)

<br/>

<img src="assets/demo-banner.jpg" alt="Xbox XDBF Explorer Architecture" width="850" style="border-radius: 8px; box-shadow: 0 4px 12px rgba(0,0,0,0.4);" />

<p><em>Systems-level binary parsing engine, memory-safe in-place serialization, and interactive Qt5 inspection suite for Xbox 360 XDBF / GPD containers.</em></p>

</div>

---

## 1. Abstract & System Architecture

The Xbox 360 operating system runtime persists identity profiles, cryptographic achievement records, stateful game parameters, and localized Unicode catalogs inside **Game Profile Data (`.gpd`) packages**. At the core of each package lies an **`XDBF` (Xbox Database File)** compound container.
# Xbox 360 XDBF Metadata Engine & Binary Serialization Pipeline

<p align="center">
  <img src="https://img.shields.io/badge/Standard-ISO%20C%2B%2B17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
  <img src="https://img.shields.io/badge/GUI-Qt%205.15%2B%20QtWidgets-41CD52?style=for-the-badge&logo=qt&logoColor=white" />
  <img src="https://img.shields.io/badge/ABI-x86__64--w64--mingw32%20(UCRT64)-2F3238?style=for-the-badge" />
  <img src="https://img.shields.io/badge/License-GPL--3.0-blue?style=for-the-badge" />
</p>

An architectural, systems-level binary parser, inspector, and injection engine engineered for Xbox 360 Database File (`XDBF`) structures and Game Profile Data (`.gpd`) compound files. 

Refactored from legacy Qt4 constructs to an ISO C++17 / Qt5 UCRT architecture, resolving strict type-aliasing hazards and providing deterministic Big-Endian to Little-Endian memory serialization.

---

## 1. Abstract & System Motivation

The Xbox 360 Xenon operating architecture manages user identity, stateful title configuration, localized string catalogs, and cryptographic achievements through discrete container files known as **Game Profile Data (`.gpd`) packages**. At the core of every valid `.gpd` container lies a contiguous, low-overhead compound binary designated as an **`XDBF` (Xbox Database File)** block.

Interfacing with these embedded console binaries presents distinct systems-level challenges:
* **Heterogeneous Endianness Constraints:** The Xenon system architecture runs strictly in **Big-Endian (`MSB:BE`)** byte order across its multi-core 64-bit PowerPC processors. Executing deterministic parsing on x86_64 host machines requires robust, low-latency endian-inversion routines across all multi-byte primitive boundaries.
* **Dynamic Sparse Heap Layouts:** To minimize IO overhead on early optical media and flash memory, XDBF records utilize table-driven pointer indirections, sparse run-length memory free tables, and fragmented payload heaps.

This framework delivers a type-safe reference parser and desktop analysis suite capable of bit-level traversal, structural integrity validation, and payload deserialization.

---

## 2. Binary Layout & Formal Specification

An XDBF file operates as a packed binary container aligned to 32-bit boundaries. 


```

+-------------------------------------------------------------------------+
|                           XDBF Compound File                            |
+=========================================================================+
| Container Header (Magic: 0x58444246, Format Version, Offsets)           |
+-------------------------------------------------------------------------+
| Free Memory Defragmentation Table (Sparse Span Allocation Headers)      |
+-------------------------------------------------------------------------+
| Master Entry Table (Compound Keys: [Namespace Type + 64-bit ID])        |
+-------------------------------------------------------------------------+
| Contiguous Payload Heap                                                 |
|  ├── Achievement Descriptors (Scores, Flags, Timestamp Vectors)         |
|  ├── UTF-16-BE Localized String Payloads                                |
|  ├── Raw PNG / DXT Image Buffers                                        |
|  └── Sync Ordering Queues & State Bitmasks                              |
+-------------------------------------------------------------------------+

```

### 2.1 File Header Layout

| Field Name | Type | Size (Bytes) | Description |
|---|---|---|---|
| `magic` | `char[4]` | 4 | File identifier signature (`0x58444246` / `"XDBF"`) |
| `version` | `uint32_t` | 4 | High/Low format revision (Big-Endian) |
| `entryTableOffset` | `uint32_t` | 4 | Absolute byte offset from file start to Entry Table |
| `entryCount` | `uint32_t` | 4 | Number of active entries registered in table |
| `freeMemTableLength` | `uint32_t` | 4 | Capacity of the free allocation table |
| `freeMemEntryCount` | `uint32_t` | 4 | Active free memory blocks tracked in container |

### 2.2 Entry Record Descriptor

Each payload record within the container is addressed via a 24-byte record descriptor:

```cpp
#pragma pack(push, 1)
struct XdbfEntryDescriptor {
    uint16_t  namespaceType;   // 1 = Achievement, 2 = Image, 3 = Setting, etc.
    uint64_t  recordId;        // Unique 64-bit Big-Endian Identifier
    uint32_t  payloadOffset;   // Byte offset relative to container origin
    uint32_t  payloadLength;   // Size of payload block in octets
    uint32_t  flags;           // Reserved allocation / sync state flags
};
#pragma pack(pop)

```

---

## 3. Engineering Mechanics & Runtime Performance

### 3.1 Strict-Aliasing Safe Endian Transformations

Direct pointer casts between mismatched primitive types (`(double*)&int64` or `(uint32_t*)&float32`) cause undefined behavior under `-O2` / `-O3` compilation flags due to compiler reordering. The engine implements vectorized intrinsic swapping:

```cpp
template <typename T>
inline void SwapEndianInPlace(T &val) {
    static_assert(std::is_trivially_copyable<T>::value, "POD type required");
    if constexpr (sizeof(T) == 2) {
        val = static_cast<T>(__builtin_bswap16(static_cast<uint16_t>(val)));
    } else if constexpr (sizeof(T) == 4) {
        val = static_cast<T>(__builtin_bswap32(static_cast<uint32_t>(val)));
    } else if constexpr (sizeof(T) == 8) {
        val = static_cast<T>(__builtin_bswap64(static_cast<uint64_t>(val)));
    }
}

```

### 3.2 Dynamic Free-Memory Compaction (`fmalloc`)

When modifying records or injecting new achievement manifests, the engine scans the in-memory free table:

1. Locates available memory blocks satisfying $\text{Length}_{\text{span}} \ge \text{Size}_{\text{payload}}$.
2. Performs split-block reallocation, writing the residual delta back into the `freeMemTable`.
3. If no contiguous block fits the footprint, memory dynamically expands at the EOF boundary, atomically updating table descriptors.

---

## 4. Modernization Architecture (Qt4 $\rightarrow$ Qt5 UCRT64)

The codebase was refactored from legacy Win32/Qt4 dependencies to native 64-bit Universal C Runtime targets:

* **ABI Transition:** Fully decoupled from `MSVCRT.DLL` in favor of Microsoft's standard `UCRT.DLL`.
* **Widget Hierarchy:** Refactored monolithic `<QtGui/*>` includes into modular `<QtWidgets/*>` pipelines.
* **Storage Path API:** Replaced deprecated `QDesktopServices::storageLocation()` calls with deterministic `QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)`.
* **Table Layout Engine:** Migrated deprecated `setResizeMode()` calls to modern `setSectionResizeMode()`.
* **String Encodings:** Replaced lossy `.toAscii()` conversions with explicit `.toLatin1()` and UTF-8 memory arrays.

---

## 5. Build & Compilation Guide

### Requirements

* **Runtime:** MSYS2 with UCRT64 toolchain (`x86_64-w64-mingw32`)
* **Compiler:** MinGW-w64 GCC 14+
* **Framework:** Qt 5.15+ (Core, Gui, Widgets, Network)

### PowerShell Build Sequence

```powershell
# 1. Isolate UCRT64 binary path
$env:Path = "C:\msys64\ucrt64\bin;C:\Windows\system32;C:\Windows"

# 2. Enter project directory
cd XDBF

# 3. Generate Qt Makefile
qmake-qt5.exe XDBF.pro

# 4. Compile binary across logical threads
mingw32-make -j4

# 5. Launch compiled executable
.\release\xbox-xdbf-explorer.exe

```

---

## 6. Project Structure

```
├── XDBF/
│   ├── XDBF.pro                  # Qt build flags and compiler directives
│   ├── xdbf.cpp / .h             # Core binary container parser and heap manager
│   ├── xdbfhelper.cpp / .h       # Low-level byte swapping routines and GUID encoders
│   ├── FileIO.cpp / .h           # In-memory file stream abstractions
│   ├── mainwindow.cpp / .h       # Visual inspection UI controller
│   ├── imagedialog.cpp / .h      # Extraction dialog for raw compressed textures & PNGs
│   ├── achievementviewer.cpp/.h  # Metadata deserializer for achievement bitfields
│   └── *.ui                      # Form definitions compiled via UIC
├── .gitignore                    # Build artifact isolation filter
└── README.md                     # Technical architecture documentation

```

---

## 7. License

Distributed under the **GNU General Public License v3.0 (GPL-3.0)**. Intended for systems reverse-engineering, digital preservation, and console file format research.

