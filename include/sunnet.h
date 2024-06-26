﻿//
// sunnet project
// Copyright (C) 2018 by Contributors <https://github.com/Tyill/sunnet>
//
// This code is licensed under the MIT License.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef SUNNET_C_API_H_
#define SUNNET_C_API_H_

#ifdef _WIN32
#ifdef SUNNETDLL_EXPORTS
#define SUNNET_API __declspec(dllexport)
#else
#define SUNNET_API __declspec(dllimport)
#endif
#else
#define SUNNET_API
#endif

#if defined(__cplusplus)
extern "C" {
namespace SN_API{
#endif /* __cplusplus */

typedef float snFloat;

/// data layer size
struct snLSize{

    size_t w, h, ch, bsz; ///< width, height, channels, batch size
    snLSize(size_t w_ = 1, size_t h_ = 1, size_t ch_ = 1, size_t bsz_ = 1) :
        w(w_), h(h_), ch(ch_), bsz(bsz_){};
};

/// object net
typedef void* sunnet;

typedef void* snUData;                                      ///< user data    
typedef void(*snStatusCBack)(const char* mess, snUData);    ///< status callback


/// version lib
/// @param[out] outVersion The memory is allocated by the user
SUNNET_API void snVersionLib(char* outVersion /*sz 32*/);

/// create net
/// @param[in] jnNet - network architecture in JSON
/// @param[out] outErr - parse error jnNet. "" - ok. The memory is allocated by the user
/// @param[in] statusCBack - callback state. Not necessary
/// @param[in] udata - user data. Not necessary
/// @return object net
SUNNET_API sunnet snCreateNet(const char* jnNet,
    char* outErr /*sz 256*/,
    snStatusCBack = nullptr,
    snUData = nullptr);

/// get last error
/// @param[in] sunnet - object net
/// @param[out] outErr - parse error jnNet. "" - ok. The memory is allocated by the user
SUNNET_API void snGetLastErrorStr(sunnet, char* outErr);

/// training - a cycle forward-back with auto-correction of weights
/// @param[in] sunnet - object net
/// @param[in] lr - learning rate
/// @param[in] isz - input layer size
/// @param[in] iLayer - input layer NCHW(bsz, ch, h, w)        
/// @param[in] osz - size of target and result. Sets for verification
/// @param[out] outData - result, the size must match the markup. The memory is allocated by the user
/// @param[in] targetData - target, the size must match the markup. The memory is allocated by the user
/// @param[out] outAccurate - current accuracy
/// @return true - ok
SUNNET_API bool snTraining(sunnet,
    snFloat lr,
    snLSize isz,
    const snFloat* iLayer,
    snLSize osz,
    snFloat* outData,
    const snFloat* targetData,
    snFloat* outAccurate = nullptr);

/// forward pass
/// @param[in] sunnet - object net
/// @param[in] isLern - is lern?
/// @param[in] isz - input layer size
/// @param[in] iLayer - input layer NCHW(bsz, ch, h, w)      
/// @param[in] osz - size of result. Sets for verification
/// @param[out] outData - result, the size must match the markup. The memory is allocated by the user
/// @return true - ok
SUNNET_API bool snForward(sunnet,
    bool isLern,
    snLSize isz,
    const snFloat* iLayer,
    snLSize osz,
    snFloat* outData);

/// backward pass
/// @param[in] sunnet - object net
/// @param[in] lr - learning rate
/// @param[in] gsz - size of the error gradient. Sets for verification
/// @param[in] grad - error gradient, the size must match the output
/// @return true - ok
SUNNET_API bool snBackward(sunnet,
    snFloat lr,
    snLSize gsz,
    const snFloat* grad);


/// set weight of node ('channels first' NCHW(bsz, ch, h, w))
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[in] wsz - size
/// @param[in] wData - weight        
/// @return true - ok
SUNNET_API bool snSetWeightNode(sunnet,
    const char* nodeName,
    snLSize wsz,
    const snFloat* wData);

/// get weight of node ('channels first' NCHW(bsz, ch, h, w))
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[out] wsz - output size
/// @param[out] wData - output data. First pass NULL, then pass it to the same 
/// @return true - ok
SUNNET_API bool snGetWeightNode(sunnet,
    const char* nodeName,
    snLSize* wsz,
    snFloat** wData);

/// batchNorm
struct batchNorm{
    snFloat* mean = nullptr;      ///< mean. The memory is allocated by the user
    snFloat* varce = nullptr;     ///< disp
    snFloat* scale = nullptr;     ///< coef γ
    snFloat* schift = nullptr;    ///< coef β
};

/// set batchNorm of node
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[in] bnsz - data size
/// @param[in] bnData - data       
/// @return true - ok
SUNNET_API bool snSetBatchNormNode(sunnet,
    const char* nodeName,
    snLSize bnsz,
    batchNorm bnData);

/// get batchNorm of node
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[out] bnsz - data size
/// @param[out] bnData - data         
/// @return true - ok
SUNNET_API bool snGetBatchNormNode(sunnet,
    const char* nodeName,
    snLSize* bnsz,
    batchNorm* bnData);

/// set input node (relevant for additional inputs)
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[in] isz - data size
/// @param[in] inData - data       
/// @return true - ok
SUNNET_API bool snSetInputNode(sunnet,
    const char* nodeName,
    snLSize isz,
    const snFloat* inData);

/// get output node (relevant for additional inputs)
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[out] osz - data size
/// @param[out] outData - data. First pass NULL, then pass it to the same 
/// @return true - ok
SUNNET_API bool snGetOutputNode(sunnet,
    const char* nodeName,
    snLSize* osz,
    snFloat** outData);

/// set gradient node (relevant for additional outputs)
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[in] gsz - data size
/// @param[in] gData - data        
/// @return true - ok
SUNNET_API bool snSetGradientNode(sunnet,
    const char* nodeName,
    snLSize gsz,
    const snFloat* gData);

/// get gradient node (relevant for additional outputs)
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[out] gsz - data size
/// @param[out] gData - data. First pass NULL, then pass it to the same 
/// @return true - ok
SUNNET_API bool snGetGradientNode(sunnet,
    const char* nodeName,
    snLSize* gsz,
    snFloat** gData);

/// set params of node
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[in] jnParam - params of node in JSON. 
/// @return true - ok
SUNNET_API bool snSetParamNode(sunnet,
    const char* nodeName,
    const char* jnParam);

/// get params of node
/// @param[in] sunnet - object net
/// @param[in] nodeName - name node
/// @param[out] jnParam - params of node in JSON 
/// @return true - ok
SUNNET_API bool snGetParamNode(sunnet,
    const char* nodeName,
    char** jnParam);

/// get architecture of net
/// @param[in] sunnet - object net
/// @param[out] jnNet - architecture of net in JSON
/// @return true - ok
SUNNET_API bool snGetArchitecNet(sunnet,
    char** jnNet);


/// userCallBack for 'userLayer' node
/// @param[in] cbname - name user cback 
/// @param[in] node - name node 
/// @param[in] fwdBwd - current action forward(true) or backward(false)
/// @param[in] insz - input layer size - receive from prev node
/// @param[in] in - input layer - receive from prev node
/// @param[out] outsz - output layer size - send to next node
/// @param[out] out - output layer - send to next node
/// @param[in] ud - aux used data
typedef void(*snUserCBack)(const char* cbname,
    const char* node,
    bool fwdBwd,
    snLSize insz,
    snFloat* in,
    snLSize* outsz,
    snFloat** out,
    snUData ud);

/// add user callBack for 'userLayer' node
/// @param[in] sunnet - object net
/// @param[in] cbName - name callBack
/// @param[in] snUserCBack - callBack
/// @param[in] snUData - user data
/// @return true - ok
SUNNET_API bool snAddUserCallBack(sunnet, const char* cbName, snUserCBack, snUData = nullptr);

/// save all weight's (and bnorm if exist) to file
/// @param[in] sunnet - object net
/// @param[in] filePath - path to file
/// @return true - ok
SUNNET_API bool snSaveAllWeightToFile(sunnet, const char* filePath);

/// load all weight's (and bnorm if exist) from file
/// @param[in] sunnet - object net
/// @param[in] filePath - path to file
/// @return true - ok
SUNNET_API bool snLoadAllWeightFromFile(sunnet, const char* filePath);

/// free object net
/// @param[in] sunnet - object net
SUNNET_API void snFreeNet(sunnet);

/// free resources
/// @param[in] data - gradient or weight from snGetWeightNode
/// @param[in] str - string from snGetArchitecNet
SUNNET_API void snFreeResources(snFloat* data = nullptr, char* str = nullptr);

#if defined(__cplusplus)
}}
#endif /* __cplusplus */

#endif /* SUNNET_C_API_H_ */