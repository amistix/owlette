package org.amistix.owlette;

import java.io.*;
import java.util.*;

public class TunnelsConfig {

    private static Map<String, Map<String, String>> readConfiguration(String tunnelsConfigFile) {
        Map<String, Map<String, String>> config = new LinkedHashMap<>();
        Map<String, String> currentSection = null;

        try (BufferedReader reader = new BufferedReader(new FileReader(tunnelsConfigFile))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#") || line.startsWith(";")) continue;

                if (line.startsWith("[") && line.endsWith("]")) {
                    String section = line.substring(1, line.length() - 1);
                    currentSection = config.computeIfAbsent(section, k -> new LinkedHashMap<>());
                } else if (line.contains("=") && currentSection != null) {
                    String[] parts = line.split("=", 2);
                    currentSection.put(parts[0].trim(), parts[1].trim());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return config;
    }

    private static void writeConfiguration(Map<String, Map<String, String>> config, String tunnelsConfigFile) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(tunnelsConfigFile))) {
            for (Map.Entry<String, Map<String, String>> section : config.entrySet()) {
                writer.write("[" + section.getKey() + "]\n");
                for (Map.Entry<String, String> entry : section.getValue().entrySet()) {
                    writer.write(entry.getKey() + "=" + entry.getValue() + "\n");
                }
                writer.write("\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void setTunnelProperty(String tunnelsConfigPath, String tunnelName, String propertyName, String propertyValue) {
        String tunnelsConfigFile = tunnelsConfigPath + "/tunnels.conf";
        Map<String, Map<String, String>> config = readConfiguration(tunnelsConfigFile);
        config.computeIfAbsent(tunnelName, k -> new LinkedHashMap<>()).put(propertyName, propertyValue);
        writeConfiguration(config, tunnelsConfigFile);
    }

    public static Set<String> getTunnels(String tunnelsConfigPath) {
        String tunnelsConfigFile = tunnelsConfigPath + "/tunnels.conf";
        return readConfiguration(tunnelsConfigFile).keySet();
    }

    public static Map<String, String> getTunnelProperties(String tunnelsConfigPath, String tunnelName) {
        String tunnelsConfigFile = tunnelsConfigPath + "/tunnels.conf";
        return readConfiguration(tunnelsConfigFile).getOrDefault(tunnelName, Collections.emptyMap());
    }
}
