// Copyright 2015-2017, SINTEF Ocean.
// Distributed under the 3-Clause BSD License.
// (See accompanying file LICENCE.txt or copy at
// https://github.com/viproma/demo-fmus/raw/master/LICENCE.txt.)

#include <cstring>
#include <cppfmu_cs.hpp>
#include "spring_1d-fmu-uuid.h"

enum
{
    VR_pos_a     = 0,
    VR_pos_b     = 1,
    VR_force_a   = 2,
    VR_force_b   = 3,
    VR_length    = 4,
    VR_stiffness = 5,
    VR_COUNT     = 6
};


class Spring1DInstance : public cppfmu::SlaveInstance
{
public:
    Spring1DInstance()
    {
        Spring1DInstance::Reset();
    }

private:
    void Reset() override
    {
        for (int i = 0; i < VR_COUNT; ++i) m_values[i] = 0.0;
        m_values[VR_pos_b]      = 1.0;
        m_values[VR_length]     = 1.0;
        m_values[VR_stiffness]  = 1.0;
    }

    void SetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        const fmiReal value[]) override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            m_values[vr[i]] = value[i];
        }
    }

    void GetReal(
        const fmiValueReference vr[],
        std::size_t nvr,
        fmiReal value[]) const override
    {
        for (std::size_t i = 0; i < nvr; ++i) {
            if (vr[i] == VR_force_a) {
                value[i] = -Force();
            } else if (vr[i] == VR_force_b) {
                value[i] = Force();
            } else {
                value[i] = m_values[vr[i]];
            }
        }
    }

    bool DoStep(
        fmiReal /*currentCommunicationPoint*/,
        fmiReal /*communicationStepSize*/,
        fmiBoolean /*newStep*/,
        fmiReal& /*endOfStep*/) override
    {
        return true;
    }

    fmiReal Force() const
    {
        const auto excitedLength = m_values[VR_pos_b] - m_values[VR_pos_a];
        const auto deltaLength = excitedLength - m_values[VR_length];
        return - m_values[VR_stiffness] * deltaLength;
    }

    fmiReal m_values[VR_COUNT];
};


cppfmu::UniquePtr<cppfmu::SlaveInstance> CppfmuInstantiateSlave(
    fmiString  /*instanceName*/,
    fmiString  fmuGUID,
    fmiString  /*fmuLocation*/,
    fmiString  /*mimeType*/,
    fmiReal    /*timeout*/,
    fmiBoolean /*visible*/,
    fmiBoolean /*interactive*/,
    cppfmu::Memory memory,
    cppfmu::Logger /*logger*/)
{
    if (std::strcmp(fmuGUID, FMU_UUID) != 0) {
        throw std::runtime_error("FMU GUID mismatch");
    }
    return cppfmu::AllocateUnique<Spring1DInstance>(memory);
}
