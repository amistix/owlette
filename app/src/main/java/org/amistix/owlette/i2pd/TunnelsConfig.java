package org.amistix.owlette.i2pd;

import java.io.*;
import java.nio.file.*;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.*;
import java.util.regex.Pattern;

import org.apache.commons.codec.binary.Base32;

public class TunnelsConfig {

    private static Map<String, Map<String, String>> readConfiguration(String tunnelsConfigFile) {
        Map<String, Map<String, String>> config = new LinkedHashMap<>();
        Map<String, String> currentSection = null;
        boolean sectionCommented = false;

        try (BufferedReader reader = new BufferedReader(new FileReader(tunnelsConfigFile))) {
            String line;

            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;

                // Section header (commented or not)
                if ((line.startsWith("[") || line.startsWith("#[") || line.startsWith(";[")) && line.endsWith("]")) {
                    sectionCommented = line.startsWith("#") || line.startsWith(";");
                    String header = line.replaceFirst("^[#;]", "");
                    String sectionName = header.substring(1, header.length() - 1);
                    currentSection = config.computeIfAbsent(sectionName, k -> new LinkedHashMap<>());

                    if (sectionCommented) {
                        currentSection.put("_commented_", "true");
                    } else {
                        currentSection.remove("_commented_");
                    }
                    continue;
                }

                // Parse key=value inside section
                if (currentSection != null && line.contains("=")) {
                    // Strip leading # or ; if section was commented
                    if (sectionCommented && (line.startsWith("#") || line.startsWith(";"))) {
                        line = line.substring(1).trim();
                    }

                    String[] parts = line.split("=", 2);
                    if (parts.length == 2) {
                        currentSection.put(parts[0].trim(), parts[1].trim());
                    }
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
                boolean commented = "true".equals(section.getValue().get("_commented_"));
                String header = "[" + section.getKey() + "]";
                if (commented) header = "#" + header;
                writer.write(header + "\n");

                for (Map.Entry<String, String> entry : section.getValue().entrySet()) {
                    if (entry.getKey().equals("_commented_")) continue; // skip internal flag
                    String line = entry.getKey() + "=" + entry.getValue();
                    if (commented) line = "#" + line;
                    writer.write(line + "\n");
                }
                writer.write("\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static Set<String> getTunnels(String tunnelsConfigPath) {
        String tunnelsConfigFile = tunnelsConfigPath + "/tunnels.conf";
        return readConfiguration(tunnelsConfigFile).keySet();
    }

    public static Map<String, String> getTunnelProperties(String tunnelsConfigPath, String tunnelName) {
        String tunnelsConfigFile = tunnelsConfigPath + "/tunnels.conf";
        return new LinkedHashMap<>(readConfiguration(tunnelsConfigFile).getOrDefault(tunnelName, Collections.emptyMap()));
    }

    public static void setTunnelProperty(String tunnelsConfigPath, String tunnelName, String propertyName, String propertyValue) {
        String tunnelsConfigFile = tunnelsConfigPath + "/tunnels.conf";
        Map<String, Map<String, String>> config = readConfiguration(tunnelsConfigFile);
        Map<String, String> section = config.computeIfAbsent(tunnelName, k -> new LinkedHashMap<>());
        section.put(propertyName, propertyValue);
        writeConfiguration(config, tunnelsConfigFile);
    }

    public static boolean isTunnelCommentedOut(String tunnelsConfigPath, String tunnelName) {
        Map<String, String> props = getTunnelProperties(tunnelsConfigPath, tunnelName);
        return "true".equals(props.get("_commented_"));
    }

    public static void commentOutTunnel(String tunnelsConfigPath, String tunnelName) {
        Map<String, Map<String, String>> config = readConfiguration(tunnelsConfigPath + "/tunnels.conf");
        Map<String, String> section = config.get(tunnelName);
        if (section != null) section.put("_commented_", "true");
        writeConfiguration(config, tunnelsConfigPath + "/tunnels.conf");
    }

    public static void uncommentTunnel(String tunnelsConfigPath, String tunnelName) {
        Map<String, Map<String, String>> config = readConfiguration(tunnelsConfigPath + "/tunnels.conf");
        Map<String, String> section = config.get(tunnelName);
        if (section != null) section.remove("_commented_");
        writeConfiguration(config, tunnelsConfigPath + "/tunnels.conf");
    }

    public static void removeTunnel(String tunnelsConfigPath, String tunnelName) {
        Map<String, Map<String, String>> config = readConfiguration(tunnelsConfigPath + "/tunnels.conf");
        config.remove(tunnelName);
        writeConfiguration(config, tunnelsConfigPath + "/tunnels.conf");
    }

    public static String getTunnelDestination(String tunnelName) throws IOException {
        return DaemonWrapper.getBase64Destination(tunnelName);
    }

    public static String getTunnelBase32(String tunnelKeyFileName) throws IOException, NoSuchAlgorithmException {
        String base64 = getTunnelDestination(tunnelKeyFileName);
        byte[] decoded = Base64.getDecoder().decode(base64);

        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        byte[] hash = digest.digest(decoded);

        Base32 base32 = new Base32();
        String b32 = base32.encodeToString(hash).toLowerCase().replace("=", "");

        return b32 + ".b32.i2p";
    }
}
