package org.amistix.owlette.i2pd;

import java.io.*;
import java.util.*;
import java.nio.file.*;
import java.security.MessageDigest;
import org.apache.commons.codec.binary.Base32;
import java.security.NoSuchAlgorithmException;


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

    public static boolean isTunnelCommentedOut(String tunnelsConfigPath, String tunnelName) {
        Path configPath = Paths.get(tunnelsConfigPath, "tunnels.conf");

        try {
            List<String> lines = Files.readAllLines(configPath);

            for (String line : lines) {
                String trimmed = line.trim();

                if (trimmed.equals("#[" + tunnelName + "]") || trimmed.equals(";[" + tunnelName + "]")) {
                    return true;
                }
                if (trimmed.equals("[" + tunnelName + "]")) {
                    return false;
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return false;
    }

    public static void commentOutTunnel(String tunnelsConfigPath, String tunnelName) {
        Path configPath = Paths.get(tunnelsConfigPath, "tunnels.conf");
        List<String> lines;
        try {
            lines = Files.readAllLines(configPath);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        List<String> newLines = new ArrayList<>();
        boolean inTargetSection = false;

        for (String line : lines) {
            String trimmed = line.trim();

            if (trimmed.equals("[" + tunnelName + "]")) {
                inTargetSection = true;
                newLines.add("#" + line);
                continue;
            }

            if (inTargetSection && trimmed.startsWith("[") && trimmed.endsWith("]")) {
                inTargetSection = false;
            }

            if (inTargetSection) {
                if (!trimmed.startsWith("#") && !trimmed.startsWith(";")) {
                    newLines.add("#" + line);
                } else {
                    newLines.add(line);
                }
            } else {
                newLines.add(line);
            }
        }

        try {
            Files.write(configPath, newLines);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void removeTunnel(String tunnelsConfigPath, String tunnelName) {
        Path configPath = Paths.get(tunnelsConfigPath, "tunnels.conf");
        List<String> lines;
        try {
            lines = Files.readAllLines(configPath);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        List<String> newLines = new ArrayList<>();
        boolean inTargetSection = false;

        for (String line : lines) {
            String trimmed = line.trim();

            if (trimmed.equals("[" + tunnelName + "]") || trimmed.equals("#[" + tunnelName + "]")) {
                inTargetSection = true;
                continue;
            }

            if (inTargetSection && (trimmed.startsWith("[") || trimmed.startsWith("#[")) && trimmed.endsWith("]")) {
                inTargetSection = false;
            }

            if (!inTargetSection) {
                newLines.add(line);
            }
        }

        try {
            Files.write(configPath, newLines);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public static String getTunnelDestination(String tunnelName)
            throws IOException{
        
        return DaemonWrapper.getBase64Destination(tunnelName);
    }

    public static String getTunnelBase32(String tunnelKeyFileName)
            throws IOException, NoSuchAlgorithmException {
        String base64 = getTunnelDestination(tunnelKeyFileName);
        byte[] decoded = Base64.getDecoder().decode(base64);

        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        byte[] hash = digest.digest(decoded);

        Base32 base32 = new Base32();
        String b32 = base32.encodeToString(hash).toLowerCase().replace("=", "");

        return b32 + ".b32.i2p";
}
}
