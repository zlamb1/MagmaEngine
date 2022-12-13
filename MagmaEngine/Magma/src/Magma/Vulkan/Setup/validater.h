#pragma once

#include <vector>

namespace Magma {

	class Validater {

	public:
		Validater() = default;

		[[nodiscard]] bool isValidationEnabled() const;
		void setValidationEnabled(bool validationEnabled);

		[[nodiscard]] size_t getLayerSize() const;
		[[nodiscard]] const char* const* getLayerData() const;

		[[nodiscard]] bool ensureRequiredLayers() const;

	private:
		bool m_ValidationEnabled = true;

		const std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

	};

}
