#pragma once

class IRenderer;

IRenderer* GetRendererByBackend( int backend );

using RenderBackendAllocator = IRenderer* ();

// =====================================================================
// Static linked list of registries
// Doesn't work with static libs for some reasons
// =====================================================================
class BackendRegistry
{
public:
	BackendRegistry( RenderBackendAllocator allocator, int backend )
		: AllocateBackend( allocator )
	{
		prev = head;
		head = this;
	}

	bool IsBackend( int backend )
	{
		return renderBackend == backend;
	}

	IRenderer* Allocate()
	{
		if ( nullptr != AllocateBackend )
		{
			return AllocateBackend();
		}
		
		return nullptr;
	}

	static IRenderer* AllocateByBackend( int backend )
	{
		BackendRegistry* reg = head;
		
		while ( reg )
		{
			if ( reg->IsBackend( backend ) )
			{
				return reg->Allocate();
			}

			reg = reg->prev;
		}

		return nullptr;
	}

private:
	static BackendRegistry* head;
	BackendRegistry* prev{ nullptr };

	int renderBackend;
	RenderBackendAllocator* AllocateBackend;
};

template< typename BackendType >
class BackendRegister
{
public:
	BackendRegister()
	{
		printf( "Registered API name: %s\n", BackendType().GetAPIName() );
	}

	static IRenderer* AllocateInstance()
	{
		static BackendRegistry registry = BackendRegistry( AllocateInstance, BackendType::BackendType );
		return new BackendType();
	}
};

/*
Copyright (c) 2021 Admer456

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
