cmake_minimum_required(VERSION 3.20)
project("Vulkan" "CXX")

if(MSVC)
  set(FLAGS
    /W4 # Wysoki poziom ostrzeżeń 
    /Wall # Wszystkie ostrzeżenia 
    /WX # Traktuj ostrzeżenia jako błędy
    /permissive- # Ścisła zgodność z C++
    /EHsc # Standardowy model wyjątków C++
    /Zi # Informacje debug 
    /GL # Optymalizacja link-time 
    /O2 # Optymalizacja kodu pod kątem szybkości
    /Oi # Inline funkcji
    /Ot # Szybszy kod 
    /Ob2 # Maksymalny poziom inline
    /Gy # Oddzielne funkcje 
    /fp:fast # Szybsze obliczenia zmiennoprzecinkowe
  )
else()
  set(FLAGS
    -Wall # Wszystkie ostrzeżenia standardowe
    -Wextra # Dodatkowe ostrzeżenia
    -Wpedantic # Zgodność ze standardem C++
    -Wconversion # Ostrzeżenia o niejawnych konwersjach
    -Wsign-conversion # Konwersje ze znakiem/bez znaku
    -Wshadow # Ostrzeżenie o cieniach zmiennych
    -Wnon-virtual-dtor # Klasy bazowe bez wirtualnego destruktora
    -Werror
    -Wold-style-cast # Rzuty w stylu C
    -Wcast-align # Ostrzeżenie o niepoprawnym wyrównaniu
    -Woverloaded-virtual # Przesłanianie metod bez "override"
    -Wdouble-promotion # Promocje float→double
    -Wformat=2 # Sprawdzanie formatów printf/scanf
    -Wundef # Użycie niezdefiniowanych makr
    -Wpointer-arith # Operacje na wskaźnikach
    -Wwrite-strings # Literale string jako const char*
    -Wlogical-op # Ostrzeżenie o błędnych operatorach logicznych
    -Wuseless-cast # Rzuty, które nic nie zmieniają
    -Wduplicated-cond # Duplikaty warunków w if
    -Wduplicated-branches # Duplikaty bloków w if/else
    -Wnull-dereference # Potencjalne dereferencje nullptr
    -Wcast-qual # Utrata kwalifikatora const
    -Wmissing-include-dirs
    -Wunreachable-code # Kod nieosiągalny
    -Wextra-semi # Zbędne średniki
    -fstrict-aliasing # Surowsze zasady aliasingu (wydajność)
    -fno-omit-frame-pointer
    -fno-common
    -O3 # Najwyższy poziom optymalizacji
    -march=native # Optymalizacja pod bieżący CPU
    -flto # Link-time optimization
    -funroll-loops # Rozwijanie pętli
    -finline-functions # Agresywne inline
    -fvisibility=hidden # Ukrywa symbole nieeksportowane
  )
endif()

include(CPack)
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(ASSAN FALSE)

FetchContent_Declare(
  glfw
  GIT_TAG master
  GIT_REPOSITORY https://github.com/glfw/glfw)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  vulkan_headers
  GIT_TAG master
  GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git)
FetchContent_MakeAvailable(vulkan_headers)

FetchContent_Declare(
  volk
  GIT_TAG master
  GIT_REPOSITORY https://github.com/zeux/volk.git)
FetchContent_MakeAvailable(volk)

find_package(Vulkan REQUIRED)

add_subdirectory("client")
