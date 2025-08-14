//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025      kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/ImageFormat/GIFEncoder.hpp>
# include <Siv3D/BinaryWriter.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/EngineLog.hpp>

# if SIV3D_PLATFORM(WINDOWS) | SIV3D_PLATFORM(MACOS) | SIV3D_PLATFORM(WEB)
#	include <ThirdParty-prebuilt/libgif/gif_lib.h>
# else
#	include <gif_lib.h>
# endif

extern "C"
{
	// libgif/libutil
# if SIV3D_PLATFORM(ANDROID)

#define COLOR_ARRAY_SIZE 32768
#define BITS_PER_PRIM_COLOR 5
#define MAX_PRIM_COLOR      0x1f
#define ABS(x)    ((x) > 0 ? (x) : (-(x)))

static int SortRGBAxis;

typedef struct QuantizedColorType {
    GifByteType RGB[3];
    GifByteType NewColorIndex;
    long Count;
    struct QuantizedColorType *Pnext;
} QuantizedColorType;

typedef struct NewColorMapType {
    GifByteType RGBMin[3], RGBWidth[3];
    unsigned int NumEntries; /* # of QuantizedColorType in linked list below */
    unsigned long Count; /* Total number of pixels in all the entries */
    QuantizedColorType *QuantizedColors;
} NewColorMapType;

/****************************************************************************
 Routine called by qsort to compare two entries.
*****************************************************************************/
static int
SortCmpRtn(const void *Entry1,
           const void *Entry2) {
    QuantizedColorType *entry1 = (*((QuantizedColorType **) Entry1));
    QuantizedColorType *entry2 = (*((QuantizedColorType **) Entry2));
    /* sort on all axes of the color space! */
    int hash1 = entry1->RGB[SortRGBAxis] * 256 * 256
                + entry1->RGB[(SortRGBAxis+1) % 3] * 256
                + entry1->RGB[(SortRGBAxis+2) % 3];
    int hash2 = entry2->RGB[SortRGBAxis] * 256 * 256
                + entry2->RGB[(SortRGBAxis+1) % 3] * 256
                + entry2->RGB[(SortRGBAxis+2) % 3];
    return hash1 - hash2;
}

/******************************************************************************
 Routine to subdivide the RGB space recursively using median cut in each
 axes alternatingly until ColorMapSize different cubes exists.
 The biggest cube in one dimension is subdivide unless it has only one entry.
 Returns GIF_ERROR if failed, otherwise GIF_OK.
*******************************************************************************/
static int
SubdivColorMap(NewColorMapType * NewColorSubdiv,
               unsigned int ColorMapSize,
               unsigned int *NewColorMapSize) {
    int MaxSize;
    unsigned int i, j, Index = 0, NumEntries, MinColor, MaxColor;
    long Sum, Count;
    QuantizedColorType *QuantizedColor, **SortArray;
    while (ColorMapSize > *NewColorMapSize) {
        /* Find candidate for subdivision: */
        MaxSize = -1;
        for (i = 0; i < *NewColorMapSize; i++) {
            for (j = 0; j < 3; j++) {
                if ((((int)NewColorSubdiv[i].RGBWidth[j]) > MaxSize) &&
                    (NewColorSubdiv[i].NumEntries > 1)) {
                    MaxSize = NewColorSubdiv[i].RGBWidth[j];
                    Index = i;
                    SortRGBAxis = j;
                }
            }
        }
        if (MaxSize == -1)
            return GIF_OK;
        /* Split the entry Index into two along the axis SortRGBAxis: */
        /* Sort all elements in that entry along the given axis and split at
         * the median.  */
        SortArray = (QuantizedColorType **)malloc(
                sizeof(QuantizedColorType *) *
                NewColorSubdiv[Index].NumEntries);
        if (SortArray == NULL)
            return GIF_ERROR;
        for (j = 0, QuantizedColor = NewColorSubdiv[Index].QuantizedColors;
             j < NewColorSubdiv[Index].NumEntries && QuantizedColor != NULL;
             j++, QuantizedColor = QuantizedColor->Pnext)
            SortArray[j] = QuantizedColor;
        /*
         * Because qsort isn't stable, this can produce differing
         * results for the order of tuples depending on platform
         * details of how qsort() is implemented.
         *
         * We mitigate this problem by sorting on all three axes rather
         * than only the one specied by SortRGBAxis; that way the instability
         * can only become an issue if there are multiple color indices
         * referring to identical RGB tuples.  Older versions of this
         * sorted on only the one axis.
         */
        qsort(SortArray, NewColorSubdiv[Index].NumEntries,
              sizeof(QuantizedColorType *), SortCmpRtn);
        /* Relink the sorted list into one: */
        for (j = 0; j < NewColorSubdiv[Index].NumEntries - 1; j++)
            SortArray[j]->Pnext = SortArray[j + 1];
        SortArray[NewColorSubdiv[Index].NumEntries - 1]->Pnext = NULL;
        NewColorSubdiv[Index].QuantizedColors = QuantizedColor = SortArray[0];
        free((char *)SortArray);
        /* Now simply add the Counts until we have half of the Count: */
        Sum = NewColorSubdiv[Index].Count / 2 - QuantizedColor->Count;
        NumEntries = 1;
        Count = QuantizedColor->Count;
        while (QuantizedColor->Pnext != NULL &&
               (Sum -= QuantizedColor->Pnext->Count) >= 0 &&
               QuantizedColor->Pnext->Pnext != NULL) {
            QuantizedColor = QuantizedColor->Pnext;
            NumEntries++;
            Count += QuantizedColor->Count;
        }
        /* Save the values of the last color of the first half, and first
         * of the second half so we can update the Bounding Boxes later.
         * Also as the colors are quantized and the BBoxes are full 0..255,
         * they need to be rescaled.
         */
        MaxColor = QuantizedColor->RGB[SortRGBAxis]; /* Max. of first half */
        /* coverity[var_deref_op] */
        MinColor = QuantizedColor->Pnext->RGB[SortRGBAxis]; /* of second */
        MaxColor <<= (8 - BITS_PER_PRIM_COLOR);
        MinColor <<= (8 - BITS_PER_PRIM_COLOR);
        /* Partition right here: */
        NewColorSubdiv[*NewColorMapSize].QuantizedColors =
                QuantizedColor->Pnext;
        QuantizedColor->Pnext = NULL;
        NewColorSubdiv[*NewColorMapSize].Count = Count;
        NewColorSubdiv[Index].Count -= Count;
        NewColorSubdiv[*NewColorMapSize].NumEntries =
                NewColorSubdiv[Index].NumEntries - NumEntries;
        NewColorSubdiv[Index].NumEntries = NumEntries;
        for (j = 0; j < 3; j++) {
            NewColorSubdiv[*NewColorMapSize].RGBMin[j] =
                    NewColorSubdiv[Index].RGBMin[j];
            NewColorSubdiv[*NewColorMapSize].RGBWidth[j] =
                    NewColorSubdiv[Index].RGBWidth[j];
        }
        NewColorSubdiv[*NewColorMapSize].RGBWidth[SortRGBAxis] =
                NewColorSubdiv[*NewColorMapSize].RGBMin[SortRGBAxis] +
                NewColorSubdiv[*NewColorMapSize].RGBWidth[SortRGBAxis] - MinColor;
        NewColorSubdiv[*NewColorMapSize].RGBMin[SortRGBAxis] = MinColor;
        NewColorSubdiv[Index].RGBWidth[SortRGBAxis] =
                MaxColor - NewColorSubdiv[Index].RGBMin[SortRGBAxis];
        (*NewColorMapSize)++;
    }
    return GIF_OK;
}
/******************************************************************************
 Quantize high resolution image into lower one. Input image consists of a
 2D array for each of the RGB colors with size Width by Height. There is no
 Color map for the input. Output is a quantized image with 2D array of
 indexes into the output color map.
   Note input image can be 24 bits at the most (8 for red/green/blue) and
 the output has 256 colors at the most (256 entries in the color map.).
 ColorMapSize specifies size of color map up to 256 and will be updated to
 real size before returning.
   Also non of the parameter are allocated by this routine.
   This function returns GIF_OK if successful, GIF_ERROR otherwise.
******************************************************************************/
int
GifQuantizeBuffer(unsigned int Width,
                  unsigned int Height,
                  int *ColorMapSize,
                  GifByteType * RedInput,
                  GifByteType * GreenInput,
                  GifByteType * BlueInput,
                  GifByteType * OutputBuffer,
                  GifColorType * OutputColorMap) {
    unsigned int Index, NumOfEntries;
    int i, j, MaxRGBError[3];
    unsigned int NewColorMapSize;
    long Red, Green, Blue;
    NewColorMapType NewColorSubdiv[256];
    QuantizedColorType *ColorArrayEntries, *QuantizedColor;
    ColorArrayEntries = (QuantizedColorType *)malloc(
            sizeof(QuantizedColorType) * COLOR_ARRAY_SIZE);
    if (ColorArrayEntries == NULL) {
        return GIF_ERROR;
    }
    for (i = 0; i < COLOR_ARRAY_SIZE; i++) {
        ColorArrayEntries[i].RGB[0] = i >> (2 * BITS_PER_PRIM_COLOR);
        ColorArrayEntries[i].RGB[1] = (i >> BITS_PER_PRIM_COLOR) &
                                      MAX_PRIM_COLOR;
        ColorArrayEntries[i].RGB[2] = i & MAX_PRIM_COLOR;
        ColorArrayEntries[i].Count = 0;
    }
    /* Sample the colors and their distribution: */
    for (i = 0; i < (int)(Width * Height); i++) {
        Index = ((RedInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                                                            (2 * BITS_PER_PRIM_COLOR)) +
                ((GreenInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                                                              BITS_PER_PRIM_COLOR) +
                (BlueInput[i] >> (8 - BITS_PER_PRIM_COLOR));
        ColorArrayEntries[Index].Count++;
    }
    /* Put all the colors in the first entry of the color map, and call the
     * recursive subdivision process.  */
    for (i = 0; i < 256; i++) {
        NewColorSubdiv[i].QuantizedColors = NULL;
        NewColorSubdiv[i].Count = NewColorSubdiv[i].NumEntries = 0;
        for (j = 0; j < 3; j++) {
            NewColorSubdiv[i].RGBMin[j] = 0;
            NewColorSubdiv[i].RGBWidth[j] = 255;
        }
    }
    /* Find the non empty entries in the color table and chain them: */
    for (i = 0; i < COLOR_ARRAY_SIZE; i++)
        if (ColorArrayEntries[i].Count > 0)
            break;
    QuantizedColor = NewColorSubdiv[0].QuantizedColors = &ColorArrayEntries[i];
    NumOfEntries = 1;
    while (++i < COLOR_ARRAY_SIZE)
        if (ColorArrayEntries[i].Count > 0) {
            QuantizedColor->Pnext = &ColorArrayEntries[i];
            QuantizedColor = &ColorArrayEntries[i];
            NumOfEntries++;
        }
    QuantizedColor->Pnext = NULL;
    NewColorSubdiv[0].NumEntries = NumOfEntries; /* Different sampled colors */
    NewColorSubdiv[0].Count = ((long)Width) * Height; /* Pixels */
    NewColorMapSize = 1;
    if (SubdivColorMap(NewColorSubdiv, *ColorMapSize, &NewColorMapSize) !=
        GIF_OK) {
        free((char *)ColorArrayEntries);
        return GIF_ERROR;
    }
    if (NewColorMapSize < *ColorMapSize) {
        /* And clear rest of color map: */
        for (i = NewColorMapSize; i < *ColorMapSize; i++)
            OutputColorMap[i].Red = OutputColorMap[i].Green =
            OutputColorMap[i].Blue = 0;
    }
    /* Average the colors in each entry to be the color to be used in the
     * output color map, and plug it into the output color map itself. */
    for (i = 0; i < NewColorMapSize; i++) {
        if ((j = NewColorSubdiv[i].NumEntries) > 0) {
            QuantizedColor = NewColorSubdiv[i].QuantizedColors;
            Red = Green = Blue = 0;
            while (QuantizedColor) {
                QuantizedColor->NewColorIndex = i;
                Red += QuantizedColor->RGB[0];
                Green += QuantizedColor->RGB[1];
                Blue += QuantizedColor->RGB[2];
                QuantizedColor = QuantizedColor->Pnext;
            }
            OutputColorMap[i].Red = (Red << (8 - BITS_PER_PRIM_COLOR)) / j;
            OutputColorMap[i].Green = (Green << (8 - BITS_PER_PRIM_COLOR)) / j;
            OutputColorMap[i].Blue = (Blue << (8 - BITS_PER_PRIM_COLOR)) / j;
        }
    }
    /* Finally scan the input buffer again and put the mapped index in the
     * output buffer.  */
    MaxRGBError[0] = MaxRGBError[1] = MaxRGBError[2] = 0;
    for (i = 0; i < (int)(Width * Height); i++) {
        Index = ((RedInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                                                            (2 * BITS_PER_PRIM_COLOR)) +
                ((GreenInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                                                              BITS_PER_PRIM_COLOR) +
                (BlueInput[i] >> (8 - BITS_PER_PRIM_COLOR));
        Index = ColorArrayEntries[Index].NewColorIndex;
        OutputBuffer[i] = Index;
        if (MaxRGBError[0] < ABS(OutputColorMap[Index].Red - RedInput[i]))
            MaxRGBError[0] = ABS(OutputColorMap[Index].Red - RedInput[i]);
        if (MaxRGBError[1] < ABS(OutputColorMap[Index].Green - GreenInput[i]))
            MaxRGBError[1] = ABS(OutputColorMap[Index].Green - GreenInput[i]);
        if (MaxRGBError[2] < ABS(OutputColorMap[Index].Blue - BlueInput[i]))
            MaxRGBError[2] = ABS(OutputColorMap[Index].Blue - BlueInput[i]);
    }
    free((char *)ColorArrayEntries);
    *ColorMapSize = NewColorMapSize;
    return GIF_OK;
}
#else

	int
	GifQuantizeBuffer(unsigned int Width,
		unsigned int Height,
		int* ColorMapSize,
		GifByteType* RedInput,
		GifByteType* GreenInput,
		GifByteType* BlueInput,
		GifByteType* OutputBuffer,
		GifColorType* OutputColorMap);
#endif


}

namespace s3d
{
	namespace detail
	{
		static int GifWriteCallback(GifFileType* gif, const GifByteType* bytes, const int size)
		{
			const auto writer = static_cast<IWriter*>(gif->UserData);

			return static_cast<int>(writer->write(bytes, size));
		}

		static int GifBlobWriteCallback(GifFileType* gif, const GifByteType* bytes, const int size)
		{
			const auto blob = static_cast<Blob*>(gif->UserData);

			blob->append(bytes, size);

			return size;
		}
	}

	StringView GIFEncoder::name() const
	{
		return U"GIF"_sv;
	}

	ImageFormat GIFEncoder::imageFormat() const noexcept
	{
		return ImageFormat::GIF;
	}

	const Array<String>& GIFEncoder::possibleExtensions() const
	{
		static const Array<String> extensions = { U"gif" };

		return extensions;
	}

	bool GIFEncoder::save(const Image& image, const FilePathView path) const
	{
		BinaryWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		return encode(image, writer);
	}

	bool GIFEncoder::encode(const Image& image, IWriter& writer) const
	{
		if (not writer.isOpen())
		{
			return false;
		}

		const int32 width = image.width();
		const int32 height = image.height();
		const int32 num_pixels = image.num_pixels();
		Array<uint8> rBuffer(num_pixels);
		Array<uint8> gBuffer(num_pixels);
		Array<uint8> bBuffer(num_pixels);

		uint8* rDst = rBuffer.data();
		uint8* gDst = gBuffer.data();
		uint8* bDst = bBuffer.data();
		{
			const Color* pSrc = image.data();
			const Color* const pSrcEnd = pSrc + num_pixels;

			while (pSrc != pSrcEnd)
			{
				*rDst++ = pSrc->r;
				*gDst++ = pSrc->g;
				*bDst++ = pSrc->b;
				++pSrc;
			}
		}

		bool hasTransparency = false;
		{
			const Color* pSrc = image.data();
			const Color* const pSrcEnd = pSrc + num_pixels;

			while (pSrc != pSrcEnd)
			{
				if (pSrc->a == 0)
				{
					hasTransparency = true;
					break;
				}

				++pSrc;
			}
		}

		int32 colorMapSize = hasTransparency ? 255 : 256;
		GifColorType colors[256] = {};

		ColorMapObject colorMap;
		colorMap.ColorCount = colorMapSize;
		colorMap.BitsPerPixel = 8;
		colorMap.Colors = colors;
		Array<GifByteType> outputBuffer(num_pixels);

		GifQuantizeBuffer(width, height, &colorMapSize,
			rBuffer.data(), gBuffer.data(), bBuffer.data(), outputBuffer.data(), colorMap.Colors);

		int32 transparencyIndex = -1;

		if (hasTransparency)
		{
			transparencyIndex = colorMap.ColorCount;

			++colorMap.ColorCount;
			colorMap.Colors[transparencyIndex] = { 0,0,0 };

			const Color* pSrc = image.data();
			const Color* const pSrcEnd = pSrc + num_pixels;
			GifByteType* pDst = outputBuffer.data();

			while (pSrc != pSrcEnd)
			{
				if (pSrc->a == 0)
				{
					*pDst = 255;
				}

				++pSrc;
				++pDst;
			}
		}

		int error = 0;
		GifFileType* gif = EGifOpen(&writer, detail::GifWriteCallback, &error);

		EGifSetGifVersion(gif, true);

		if (EGifPutScreenDesc(gif, width, height, 8, 0, nullptr) == GIF_ERROR)
		{
			EGifCloseFile(gif, &error);
			return false;
		}

		GraphicsControlBlock controlBlock;
		controlBlock.DisposalMode = DISPOSAL_UNSPECIFIED;
		controlBlock.UserInputFlag = false;
		controlBlock.DelayTime = 100;
		controlBlock.TransparentColor = hasTransparency ? transparencyIndex : NO_TRANSPARENT_COLOR;

		GifByteType ext[4];
		EGifGCBToExtension(&controlBlock, ext);

		if (EGifPutExtension(gif, GRAPHICS_EXT_FUNC_CODE, sizeof(ext), ext) == GIF_ERROR)
		{
			EGifCloseFile(gif, &error);
			return false;
		}

		int r = EGifPutImageDesc(gif, 0, 0, width, height, false, &colorMap);

		if (r != GIF_OK)
		{
			EGifCloseFile(gif, &error);
			return false;
		}

		for (int32 y = 0; y < height; ++y)
		{
			if (EGifPutLine(gif, &outputBuffer[y * width], width) == GIF_ERROR)
			{
				EGifCloseFile(gif, &error);
				return false;
			}
		}

		EGifCloseFile(gif, &error);

		return true;
	}

	Blob GIFEncoder::encode(const Image& image) const
	{
		const int32 width = image.width();
		const int32 height = image.height();
		const int32 num_pixels = image.num_pixels();
		Array<uint8> rBuffer(num_pixels);
		Array<uint8> gBuffer(num_pixels);
		Array<uint8> bBuffer(num_pixels);

		uint8* rDst = rBuffer.data();
		uint8* gDst = gBuffer.data();
		uint8* bDst = bBuffer.data();
		{
			const Color* pSrc = image.data();
			const Color* const pSrcEnd = pSrc + num_pixels;

			while (pSrc != pSrcEnd)
			{
				*rDst++ = pSrc->r;
				*gDst++ = pSrc->g;
				*bDst++ = pSrc->b;
				++pSrc;
			}
		}

		bool hasTransparency = false;
		{
			const Color* pSrc = image.data();
			const Color* const pSrcEnd = pSrc + num_pixels;

			while (pSrc != pSrcEnd)
			{
				if (pSrc->a == 0)
				{
					hasTransparency = true;
					break;
				}

				++pSrc;
			}
		}

		int32 colorMapSize = hasTransparency ? 255 : 256;
		GifColorType colors[256] = {};

		ColorMapObject colorMap;
		colorMap.ColorCount = colorMapSize;
		colorMap.BitsPerPixel = 8;
		colorMap.Colors = colors;
		Array<GifByteType> outputBuffer(num_pixels);

		GifQuantizeBuffer(width, height, &colorMapSize,
			rBuffer.data(), gBuffer.data(), bBuffer.data(), outputBuffer.data(), colorMap.Colors);

		int32 transparencyIndex = -1;

		if (hasTransparency)
		{
			transparencyIndex = colorMap.ColorCount;

			++colorMap.ColorCount;
			colorMap.Colors[transparencyIndex] = { 0,0,0 };

			const Color* pSrc = image.data();
			const Color* const pSrcEnd = pSrc + num_pixels;
			GifByteType* pDst = outputBuffer.data();

			while (pSrc != pSrcEnd)
			{
				if (pSrc->a == 0)
				{
					*pDst = 255;
				}

				++pSrc;
				++pDst;
			}
		}

		Blob blob;
		int error = 0;
		GifFileType* gif = EGifOpen(&blob, detail::GifBlobWriteCallback, &error);

		EGifSetGifVersion(gif, true);

		if (EGifPutScreenDesc(gif, width, height, 8, 0, nullptr) == GIF_ERROR)
		{
			EGifCloseFile(gif, &error);
			return{};
		}

		GraphicsControlBlock controlBlock;
		controlBlock.DisposalMode = DISPOSAL_UNSPECIFIED;
		controlBlock.UserInputFlag = false;
		controlBlock.DelayTime = 100;
		controlBlock.TransparentColor = hasTransparency ? transparencyIndex : NO_TRANSPARENT_COLOR;

		GifByteType ext[4];
		EGifGCBToExtension(&controlBlock, ext);

		if (EGifPutExtension(gif, GRAPHICS_EXT_FUNC_CODE, sizeof(ext), ext) == GIF_ERROR)
		{
			EGifCloseFile(gif, &error);
			return{};
		}

		int r = EGifPutImageDesc(gif, 0, 0, width, height, false, &colorMap);

		if (r != GIF_OK)
		{
			EGifCloseFile(gif, &error);
			return{};
		}

		for (int32 y = 0; y < height; ++y)
		{
			if (EGifPutLine(gif, &outputBuffer[y * width], width) == GIF_ERROR)
			{
				EGifCloseFile(gif, &error);
				return{};
			}
		}

		EGifCloseFile(gif, &error);

		return blob;
	}
}
